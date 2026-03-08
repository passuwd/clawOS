#pragma once

#include <stdint.h>
#include <stddef.h>

namespace PMM {

void init(uint32_t mb2_info);
void* alloc_page();
void free_page(void* ptr);

} // namespace PMM