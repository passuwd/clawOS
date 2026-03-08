.code64
.section .boot.text, "ax"
.global start64
start64:
    mov $16, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    movabs $stack_top, %rsp

    /* EDI and ESI already contain magic and mb2_info from 32-bit mode */
    /* Wait, the upper 32 bits might be garbage, let's clear them just in case */
    mov %edi, %edi
    mov %esi, %esi

    /* Since we're in higher half, call kernel_main */
    movabs $kernel_main, %rax
    call *%rax

1:  hlt
    jmp 1b