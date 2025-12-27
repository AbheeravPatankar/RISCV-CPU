MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

ENTRY(_start)

SECTIONS
{
    /* -------------------------------------------------- */
    /* Kernel loaded at 0x80000000 onward                 */
    /* -------------------------------------------------- */
    . = 0x80000000;

    /* Kernel entry point */
    .text.entry :
    {
        KEEP(*(.text.entry))
    } > RAM

    /* Kernel text */
    .text :
    {
        *(.text*)
    } > RAM

    /* Read-only data */
    .rodata :
    {
        . = ALIGN(0x10);
        *(.rodata*)
        *(.srodata*)
    } > RAM

    /* Initialized data */
    .data :
    {
        . = ALIGN(0x1000);
        *(.data*)
        *(.sdata*)
    } > RAM

    /* Zero-initialized data */
    .bss :
    {
        . = ALIGN(0x100);
        *(.bss*)
        *(.sbss*)
        *(COMMON)
    } > RAM

    /* -------------------------------------------------- */
    /* End-of-kernel markers                             */
    /* -------------------------------------------------- */
    . = ALIGN(0x10);
    _end = .;

    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
