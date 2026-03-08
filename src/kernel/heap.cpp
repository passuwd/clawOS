#include <stddef.h>
#include "pmm.h"
#include "vmm.h"

void* operator new(size_t size) {
    // Very simple bump allocator using pages from PMM for now.
    // In a real OS, use a proper dlmalloc or slab.
    return PMM::alloc_page(); 
}

void* operator new[](size_t size) {
    return PMM::alloc_page();
}

void operator delete(void* p) noexcept {
    if (p) PMM::free_page(p);
}

void operator delete[](void* p) noexcept {
    if (p) PMM::free_page(p);
}

void operator delete(void* p, size_t size) noexcept {
    if (p) PMM::free_page(p);
}

void operator delete[](void* p, size_t size) noexcept {
    if (p) PMM::free_page(p);
}
