/* vanilla_safe.ld - safe for QEMU bare-metal RISC-V */

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

/* Entry point */
ENTRY(_start)

SECTIONS
{
    /* Start of RAM */
    . = ORIGIN(RAM);

    /* Text section */
    .text :
    {
        __text_start = .;
        KEEP(*(.text.entry))      /* keep entry.S _start first */
        *(.text*)                  /* all other code */
        __text_end = .;
    }

    /* Read-only data */
    .rodata :
    {
        . = ALIGN(0x10);
        __rodata_start = .;
        *(.rodata*)
        *(.srodata*)
        __rodata_end = .;
    }

    /* Initialized data */
    .data :
    {
        . = ALIGN(0x1000);
        __data_start = .;
        *(.data*)
        *(.sdata*)
        __data_end = .;
    }

    /* Zero-initialized data */
    .bss :
    {
        . = ALIGN(0x100);
        __bss_start = .;
        *(.bss*)
        *(.sbss*)
        __bss_end = .;
    }

    /* Stack pointer top (safe for QEMU) */
    _stack_top = ORIGIN(RAM) + LENGTH(RAM);

    /* End of program */
    _end = .;
}
