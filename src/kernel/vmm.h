#pragma once

#include <stdint.h>
#include <stddef.h>

namespace VMM {

#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITE    (1 << 1)
#define PAGE_USER     (1 << 2)

void init();
uint64_t* create_pml4();
void map_page(uint64_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
void switch_pml4(uint64_t* pml4);

extern uint64_t* current_pml4;

} // namespace VMM