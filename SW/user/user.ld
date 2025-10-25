OUTPUT_ARCH(riscv)
ENTRY(_entry)
SECTIONS
{
    /* --- Code (.text) section starts at 0x0000 --- */
    .text 0x00000000 : ALIGN(32)
    {
        *(.text*)
    }
    
    /* --- Read-only data (.rodata) follows .text --- */
    .rodata : ALIGN(4)
    {
        *(.rodata*)
        *(.rodata.*)
        *(.srodata*)      /* Small read-only data */
        *(.srodata.*)
    }
    
    /* --- Small Data starts at 0x00000400 (1024 decimal) --- */
    .sdata  : ALIGN(4)
    {
        __global_pointer$ = . + 0x800;  /* GP register base (optional) */
        *(.sdata*)
        *(.sdata.*)
    }
    
    /* --- Initialized Data follows .sdata --- */
    .data : ALIGN(4)
    {
        *(.data*)
        *(.data.*)
    }
    
    /* --- Small Uninitialized Data follows .data --- */
    .sbss (NOLOAD) : ALIGN(4)
    {
        *(.sbss*)
        *(.sbss.*)
        *(.scommon)
    }
    
    /* --- Uninitialized Data follows .sbss --- */
    .bss (NOLOAD) : ALIGN(4)
    {
        *(.bss*)
        *(.bss.*)
        *(COMMON)
    }
}