#pragma once

#include <stdint.h>
#include <stddef.h>

namespace VFS {

struct Node {
    char name[256];
    uint32_t flags;
    uint32_t length;
    uint32_t inode;
    struct Node* parent;
    struct Node* next;
    struct Node* first_child;
};

void init();
Node* get_root();
void mkdir(const char* path);

} // namespace VFS