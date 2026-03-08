#include "vfs.h"
#include "serial.h"

namespace VFS {

static Node root;
static Node* last_child = nullptr;

void init() {
    __builtin_memset(&root, 0, sizeof(Node));
    root.name[0] = '/';
    root.flags = 1; // Directory
    
    Serial::write_string("VFS initialized.\n");
    
    // Create FHS structure
    mkdir("/bin");
    mkdir("/etc");
    mkdir("/home");
    mkdir("/lib");
    mkdir("/root");
    mkdir("/tmp");
    mkdir("/usr");
    mkdir("/var");
    mkdir("/boot");
    
    Serial::write_string("FHS structure created.\n");
}

Node* get_root() {
    return &root;
}

void mkdir(const char* path) {
    if (path[0] != '/') return;
    
    Node* new_node = new Node();
    __builtin_memset(new_node, 0, sizeof(Node));
    
    // Basic path handling for root level
    for (int i = 0; path[i+1] != '\0' && i < 255; i++) {
        new_node->name[i] = path[i+1];
    }
    
    new_node->flags = 1; // Directory
    new_node->parent = &root;
    
    if (root.first_child == nullptr) {
        root.first_child = new_node;
    } else {
        last_child->next = new_node;
    }
    last_child = new_node;
    
    Serial::write_string("Created directory: ");
    Serial::write_string(path);
    Serial::write_string("\n");
}

} // namespace VFS