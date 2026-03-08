#pragma once

#include <stdint.h>

namespace ExFAT {

struct BootSector {
    uint8_t jmp[3];
    uint8_t fs_name[8];
    uint8_t zero[53];
    uint64_t partition_offset;
    uint64_t volume_length;
    uint32_t fat_offset;
    uint32_t fat_length;
    uint32_t cluster_heap_offset;
    uint32_t cluster_count;
    uint32_t root_dir_cluster;
    uint32_t volume_serial;
    uint16_t fs_revision;
    uint16_t flags;
    uint8_t bytes_per_sector_log2;
    uint8_t sectors_per_cluster_log2;
    uint8_t number_of_fats;
    uint8_t drive_select;
    uint8_t percent_in_use;
    uint8_t reserved[7];
    uint8_t boot_code[390];
    uint16_t boot_signature;
} __attribute__((packed));

void init();

} // namespace ExFAT