ENTRY(start)

SECTIONS
{
    /* The kernel boot section will live at 1MB */
    . = 1M;
    
    .boot : {
        KEEP(*(.multiboot_header))
        *(.boot)
        *(.boot.text)
        *(.boot.data)
    }

    .boot.bss : {
        *(.boot.bss)
    }

    /* Move to higher half */
    . += 0xffffffff80000000;

    .text ALIGN(4K) : AT(ADDR(.text) - 0xffffffff80000000)
    {
        *(.text*)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xffffffff80000000)
    {
        *(.rodata*)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xffffffff80000000)
    {
        *(.data*)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xffffffff80000000)
    {
        *(COMMON)
        *(.bss*)
        . = ALIGN(4K);
        stack_bottom = .;
        . += 16K;
        stack_top = .;
    }
}