#pragma once

#include <stdint.h>
#include <stddef.h>

namespace VMM {

static constexpr uint64_t PAGE_PRESENT = (1 << 0);
static constexpr uint64_t PAGE_WRITE   = (1 << 1);
static constexpr uint64_t PAGE_USER    = (1 << 2);

void init();
uint64_t* create_pml4();
void map_page(uint64_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
void switch_pml4(uint64_t* pml4);

extern uint64_t* current_pml4;

} // namespace VMM