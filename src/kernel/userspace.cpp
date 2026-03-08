#include "userspace.h"
#include "vmm.h"
#include "pmm.h"
#include "serial.h"

extern "C" void jump_to_user_asm(void* entry_point, void* user_stack);

namespace Userspace {

void jump_to_user(void* entry_point, void* user_stack) {
    // We already mapped kernel higher-half.
    // Let's map the user code and stack.
    
    // In a real OS, this would be in its own address space.
    // For this demonstration, we map it into the current one.
    
    // Identity map the entry point and stack in the lower half for testing.
    VMM::map_page(VMM::current_pml4, (uint64_t)entry_point, (uint64_t)entry_point, VMM::PAGE_USER | VMM::PAGE_WRITE);
    VMM::map_page(VMM::current_pml4, (uint64_t)user_stack - 4096, (uint64_t)user_stack - 4096, VMM::PAGE_USER | VMM::PAGE_WRITE);
    
    Serial::write_string("Jumping to user mode...\n");
    jump_to_user_asm(entry_point, user_stack);
}

} // namespace Userspace