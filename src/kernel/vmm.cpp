#include "vmm.h"
#include "pmm.h"
#include "serial.h"

namespace VMM {

uint64_t* current_pml4 = nullptr;

static void memset_page(void* ptr) {
    uint64_t* p = (uint64_t*)((uint64_t)ptr + 0xffffffff80000000);
    for (int i = 0; i < 512; i++) {
        p[i] = 0;
    }
}

static uint64_t* get_next_level(uint64_t* table, size_t index, uint64_t flags) {
    if (table[index] & PAGE_PRESENT) {
        return (uint64_t*)((table[index] & 0x000FFFFFFFFFF000) + 0xffffffff80000000);
    }
    
    void* new_table = PMM::alloc_page();
    if (!new_table) return nullptr;
    
    memset_page(new_table);
    table[index] = (uint64_t)new_table | flags | PAGE_PRESENT;
    
    return (uint64_t*)((uint64_t)new_table + 0xffffffff80000000);
}

void map_page(uint64_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    size_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    size_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    size_t pd_idx = (virtual_addr >> 21) & 0x1FF;
    size_t pt_idx = (virtual_addr >> 12) & 0x1FF;

    uint64_t* pml4_v = (uint64_t*)((uint64_t)pml4 + 0xffffffff80000000);
    uint64_t* pdpt = get_next_level(pml4_v, pml4_idx, flags);
    uint64_t* pd = get_next_level(pdpt, pdpt_idx, flags);
    uint64_t* pt = get_next_level(pd, pd_idx, flags);

    pt[pt_idx] = physical_addr | flags | PAGE_PRESENT;
}

void switch_pml4(uint64_t* pml4) {
    current_pml4 = pml4;
    asm volatile("mov %0, %%cr3" : : "r" (pml4));
}

uint64_t* create_pml4() {
    uint64_t* pml4 = (uint64_t*)PMM::alloc_page();
    memset_page(pml4);
    
    // Copy kernel mappings from current PML4
    uint64_t* pml4_v = (uint64_t*)((uint64_t)pml4 + 0xffffffff80000000);
    uint64_t* current_v = (uint64_t*)((uint64_t)current_pml4 + 0xffffffff80000000);
    
    // Usually map the higher half (index 256 to 511)
    for (int i = 256; i < 512; i++) {
        pml4_v[i] = current_v[i];
    }
    
    return pml4;
}

void init() {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r" (cr3));
    current_pml4 = (uint64_t*)cr3;
    Serial::write_string("VMM initialized.\n");
}

} // namespace VMM