#include "pmm.h"
#include "serial.h"

namespace PMM {

static uint8_t* bitmap = nullptr;
static size_t bitmap_size = 0;
static size_t total_blocks = 0;
static size_t used_blocks = 0;
static size_t last_alloc_index = 0;

#define PAGE_SIZE 4096
#define BLOCKS_PER_BYTE 8

static void set_bit(size_t bit) {
    bitmap[bit / BLOCKS_PER_BYTE] |= (1 << (bit % BLOCKS_PER_BYTE));
}

static void clear_bit(size_t bit) {
    bitmap[bit / BLOCKS_PER_BYTE] &= ~(1 << (bit % BLOCKS_PER_BYTE));
}

static bool test_bit(size_t bit) {
    return bitmap[bit / BLOCKS_PER_BYTE] & (1 << (bit % BLOCKS_PER_BYTE));
}

void init(uint32_t mb2_info) {
    // For simplicity, we just place the bitmap at a known safe location (e.g. 0x200000)
    // and assume 128MB of RAM for initial setup.
    // Real implementation would parse Multiboot2 memory map tags.
    
    total_blocks = (128 * 1024 * 1024) / PAGE_SIZE; // 128 MB 
    bitmap_size = total_blocks / BLOCKS_PER_BYTE;
    bitmap = (uint8_t*)0xffffffff80200000; // Place bitmap at end of kernel mapping roughly
    
    // Initialize all memory to free
    for (size_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }
    
    // Mark first 4MB as used (kernel + boot structures + bitmap)
    for (size_t i = 0; i < (4 * 1024 * 1024) / PAGE_SIZE; i++) {
        set_bit(i);
    }
    
    used_blocks = (4 * 1024 * 1024) / PAGE_SIZE;
    Serial::write_string("PMM initialized (Simple 128MB).\n");
}

void* alloc_page() {
    for (size_t i = last_alloc_index; i < total_blocks; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            used_blocks++;
            last_alloc_index = i;
            return (void*)(i * PAGE_SIZE);
        }
    }
    for (size_t i = 0; i < last_alloc_index; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            used_blocks++;
            last_alloc_index = i;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return nullptr; // Out of memory
}

void free_page(void* ptr) {
    size_t i = (uint64_t)ptr / PAGE_SIZE;
    if (i < total_blocks && test_bit(i)) {
        clear_bit(i);
        used_blocks--;
    }
}

} // namespace PMM