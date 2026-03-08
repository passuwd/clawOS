#include "isr.h"
#include "serial.h"
#include "io.h"

const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

extern "C" void isr_handler(Registers* regs) {
    Serial::write_string("Exception: ");
    if (regs->int_no < 32) {
        Serial::write_string(exception_messages[regs->int_no]);
    } else {
        Serial::write_string("Unknown");
    }
    Serial::write_string(" (");
    Serial::write_hex(regs->int_no);
    Serial::write_string(")\n");

    Serial::write_string("Error code: ");
    Serial::write_hex(regs->err_code);
    Serial::write_string("\n");
    
    Serial::write_string("RIP: ");
    Serial::write_hex(regs->rip);
    Serial::write_string("\n");

    while (1) {
        asm volatile("hlt");
    }
}

extern "C" void irq_handler(Registers* regs) {
    // Send EOI to PICs
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}
