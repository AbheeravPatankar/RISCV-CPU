OUTPUT_ARCH(riscv)
ENTRY(_entry)

SECTIONS
{
    /* --- Code (.text) section starts at 0x0000 --- */
    .text 0x00000000 : ALIGN(32)
    {
        *(.text*)
        *(.rodata*)
    }

    /* --- Small Data starts at 0x00000400 (1024 decimal) --- */
    .sdata 0x00000400 :
    {
        *(.sdata*)
    }

    /* --- Initialized Data follows .sdata --- */
    .data : ALIGN(4)
    {
        *(.data*)
    }

    /* --- Uninitialized Data follows .data --- */
    .bss (NOLOAD) : ALIGN(4)
    {
        *(.bss*)
        *(COMMON)
    }
}
