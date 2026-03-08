.code32
.section .multiboot_header, "a"
.align 8
header_start:
    .long 0xe85250d6                /* magic */
    .long 0                         /* architecture i386 */
    .long header_end - header_start /* header length */
    /* checksum */
    .long -(0xe85250d6 + 0 + (header_end - header_start))

    /* Framebuffer tag */
    .align 8
    .short 5     /* type */
    .short 0     /* flags */
    .long 20     /* size */
    .long 1024   /* width */
    .long 768    /* height */
    .long 32     /* depth */

    /* End tag */
    .align 8
    .short 0
    .short 0
    .long 8
header_end:

.section .boot, "ax"
.global start
start:
    cli

    mov %eax, %edi
    mov %ebx, %esi

    /* Map first 4GB using 1GB pages in p3_table */
    /* p4_table[0] = p3_table */
    mov $p3_table, %eax
    or $3, %eax
    mov %eax, p4_table

    /* p3_table[0] = 0x00000000 | 0x83 */
    mov $0x83, %eax
    mov %eax, p3_table
    /* p3_table[1] = 0x40000000 | 0x83 */
    mov $0x40000083, %eax
    mov %eax, p3_table + 8
    /* p3_table[2] = 0x80000000 | 0x83 */
    mov $0x80000083, %eax
    mov %eax, p3_table + 16
    /* p3_table[3] = 0xC0000000 | 0x83 */
    mov $0xC0000083, %eax
    mov %eax, p3_table + 24

    /* Higher half: 0xffffffff80000000 -> p4[511], p3[510] */
    mov $p3_hh_table, %eax
    or $3, %eax
    mov %eax, p4_table + 511 * 8

    /* p3_hh_table[510] maps to first 1GB -> 0x00000000 | 0x83 */
    mov $0x83, %eax
    mov %eax, p3_hh_table + 510 * 8

    /* Load CR3 */
    mov $p4_table, %eax
    mov %eax, %cr3

    /* Enable PAE */
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    /* Enable long mode */
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr

    /* Enable paging */
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    lgdt gdt64_pointer
    ljmp $8, $start64

.section .boot.bss, "aw", @nobits
.align 4096
p4_table:
    .skip 4096
p3_table:
    .skip 4096
p3_hh_table:
    .skip 4096

.section .boot.data, "aw"
.align 8
gdt64:
    .quad 0
    .quad 0x00209A0000000000 /* Code 64 */
    .quad 0x0000920000000000 /* Data 64 */
gdt64_pointer:
    .word gdt64_pointer - gdt64 - 1
    .long gdt64