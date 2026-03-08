#include "gdt.h"

namespace GDT {

struct alignas(0x1000) GDTTable {
    Descriptor null_desc;
    Descriptor kernel_code;
    Descriptor kernel_data;
    Descriptor user_data;
    Descriptor user_code;
    TSSDescriptor tss;
} __attribute__((packed));

static GDTTable gdt;
static TSSEntry tss;
static GDTRegister gdtr;

extern "C" void gdt_flush(uint64_t gdtr_addr);
extern "C" void tss_flush();

void init() {
    // Null descriptor
    gdt.null_desc = {0, 0, 0, 0, 0, 0};

    // Kernel Code (Ring 0, 64-bit)
    gdt.kernel_code = {0, 0, 0, 0x9A, 0x20, 0};

    // Kernel Data (Ring 0)
    gdt.kernel_data = {0, 0, 0, 0x92, 0x00, 0};

    // User Data (Ring 3)
    gdt.user_data = {0, 0, 0, 0xF2, 0x00, 0};

    // User Code (Ring 3, 64-bit)
    gdt.user_code = {0, 0, 0, 0xFA, 0x20, 0};

    // TSS
    uint64_t tss_base = (uint64_t)&tss;
    uint32_t tss_limit = sizeof(tss) - 1;

    gdt.tss.limit_low = tss_limit & 0xFFFF;
    gdt.tss.base_low = tss_base & 0xFFFF;
    gdt.tss.base_middle = (tss_base >> 16) & 0xFF;
    gdt.tss.access = 0x89;
    gdt.tss.granularity = 0x00;
    gdt.tss.base_high = (tss_base >> 24) & 0xFF;
    gdt.tss.base_upper = (tss_base >> 32) & 0xFFFFFFFF;
    gdt.tss.reserved = 0;

    // Initialize TSS
    __builtin_memset(&tss, 0, sizeof(tss));
    tss.iopb_offset = sizeof(tss); // Disable IOPB

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    gdt_flush((uint64_t)&gdtr);
    tss_flush();
}

void set_tss_rsp0(uint64_t rsp0) {
    tss.rsp0 = rsp0;
}

} // namespace GDT