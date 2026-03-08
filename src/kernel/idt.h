#pragma once

#include <stdint.h>

namespace IDT {

struct Entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct Register {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void init();

} // namespace IDT