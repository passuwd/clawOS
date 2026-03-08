#pragma once

#include <stddef.h>

namespace Serial {
    void init();
    void write_char(char a);
    void write_string(const char* str);
    void write_hex(unsigned long val);
}