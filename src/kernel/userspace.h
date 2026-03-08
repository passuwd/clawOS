#pragma once

#include <stdint.h>

namespace Userspace {
    void jump_to_user(void* entry_point, void* user_stack);
}