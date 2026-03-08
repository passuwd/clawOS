#include "idt.h"
#include "serial.h"

namespace IDT {

__attribute__((aligned(0x10)))
static Entry idt[256];
static Register idtr;

extern "C" uint64_t isr_stub_table[];

void set_entry(int n, uint64_t isr, uint8_t flags) {
    idt[n].offset_low = isr & 0xFFFF;
    idt[n].selector = 0x08; // Kernel Code Segment
    idt[n].ist = 0;
    idt[n].flags = flags;
    idt[n].offset_middle = (isr >> 16) & 0xFFFF;
    idt[n].offset_high = (isr >> 32) & 0xFFFFFFFF;
    idt[n].reserved = 0;
}

void init() {
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt) - 1;

    for (int i = 0; i < 256; i++) {
        set_entry(i, isr_stub_table[i], 0x8E); // Interrupt Gate, Ring 0
    }

    asm volatile ("lidt %0" : : "m"(idtr));
    asm volatile ("sti");
    
    Serial::write_string("IDT initialized.\n");
}

} // namespace IDT