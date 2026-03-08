#!/usr/bin/env python3

with open('src/kernel/isr_asm.S', 'w') as f:
    f.write('.code64\n.section .text\n\n')
    f.write('.extern isr_handler\n.extern irq_handler\n\n')
    
    # Generate ISR stubs
    for i in range(256):
        f.write(f'.global isr_stub_{i}\n')
        f.write(f'isr_stub_{i}:\n')
        
        # Push dummy error code if the exception doesn't push one
        if i not in [8, 10, 11, 12, 13, 14, 17, 21, 29, 30]:
            f.write('    pushq $0\n')
            
        f.write(f'    pushq ${i}\n')
        f.write('    jmp isr_common_stub\n\n')

    # Common stub
    f.write('isr_common_stub:\n')
    f.write('    pushq %rax\n')
    f.write('    pushq %rbx\n')
    f.write('    pushq %rcx\n')
    f.write('    pushq %rdx\n')
    f.write('    pushq %rsi\n')
    f.write('    pushq %rdi\n')
    f.write('    pushq %rbp\n')
    f.write('    pushq %r8\n')
    f.write('    pushq %r9\n')
    f.write('    pushq %r10\n')
    f.write('    pushq %r11\n')
    f.write('    pushq %r12\n')
    f.write('    pushq %r13\n')
    f.write('    pushq %r14\n')
    f.write('    pushq %r15\n')
    
    f.write('    mov %rsp, %rdi\n') # Pass pointer to registers as argument
    f.write('    cld\n') # System V ABI requires DF to be clear
    
    # Call handler
    f.write('    cmpq $32, 120(%rsp)\n') # Check int_no
    f.write('    jae 1f\n')
    f.write('    call isr_handler\n')
    f.write('    jmp 2f\n')
    f.write('1:\n')
    f.write('    call irq_handler\n')
    f.write('2:\n')
    
    # Restore registers
    f.write('    popq %r15\n')
    f.write('    popq %r14\n')
    f.write('    popq %r13\n')
    f.write('    popq %r12\n')
    f.write('    popq %r11\n')
    f.write('    popq %r10\n')
    f.write('    popq %r9\n')
    f.write('    popq %r8\n')
    f.write('    popq %rbp\n')
    f.write('    popq %rdi\n')
    f.write('    popq %rsi\n')
    f.write('    popq %rdx\n')
    f.write('    popq %rcx\n')
    f.write('    popq %rbx\n')
    f.write('    popq %rax\n')
    f.write('    add $16, %rsp\n') # Clean up error code and int_no
    f.write('    iretq\n\n')
    
    # Generate array of pointers to stubs
    f.write('.section .data\n')
    f.write('.global isr_stub_table\n')
    f.write('isr_stub_table:\n')
    for i in range(256):
        f.write(f'    .quad isr_stub_{i}\n')
