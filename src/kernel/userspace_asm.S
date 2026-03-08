.code64
.section .text

.global jump_to_user_asm
jump_to_user_asm:
    /* rdi = entry_point, rsi = user_stack */
    
    mov $0x1B, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    pushq $0x1B      /* SS */
    pushq %rsi       /* RSP */
    pushq $0x202     /* RFLAGS (Interrupts enabled) */
    pushq $0x23      /* CS */
    pushq %rdi       /* RIP */
    
    iretq
