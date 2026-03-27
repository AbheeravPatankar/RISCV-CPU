OUTPUT_ARCH(riscv)
ENTRY(_entry)

SECTIONS
{
    /* -------------------------------------------------- */
    /* User program starts at VA = 0x2000 (8 KB)          */
    /* Page 0: trampoline (0x0000–0x0FFF)                 */
    /* Page 1: trapframe  (0x1000–0x1FFF)                 */
    /* -------------------------------------------------- */
    . = 0x2000;

    /* --- Code (.text) --- */
    .text : ALIGN(16)
    {
        *(.text*)
    }

    /* --- Read-only data --- */
    .rodata : ALIGN(4)
    {
        *(.rodata*)
        *(.rodata.*)
        *(.srodata*)
        *(.srodata.*)
    }

    /* --- Small initialized data --- */
    .sdata : ALIGN(4)
    {
        __global_pointer$ = . + 0x800;
        *(.sdata*)
        *(.sdata.*)
    }

    /* --- Initialized data --- */
    .data : ALIGN(4)
    {
        *(.data*)
        *(.data.*)
    }

    /* --- Small uninitialized data --- */
    .sbss (NOLOAD) : ALIGN(4)
    {
        *(.sbss*)
        *(.sbss.*)
        *(.scommon)
    }

    /* --- Uninitialized data --- */
    .bss (NOLOAD) : ALIGN(4)
    {
        *(.bss*)
        *(.bss.*)
        *(COMMON)
    }

    /* --- Heap --- */
    .heap (NOLOAD) : ALIGN(4096)
    {
        _heap_start = .;              
        . = . + 0x100000;             
        _heap_end = .;                
    }
}