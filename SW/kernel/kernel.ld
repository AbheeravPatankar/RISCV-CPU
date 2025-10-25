/* vanilla_safe_trampsec.ld
   Places trampsec (without dot) after .bss, aligned to 1024 (0x400).
   Keeps only the exact section name trampsec (via KEEP(*(trampsec))).
   Defines _stack_top and _end for runtime use.
*/
MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}
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
        *(.text*)                 /* all other code */
        __text_end = .;
    } > RAM
    
    /* Read-only data */
    .rodata :
    {
        . = ALIGN(0x10);
        __rodata_start = .;
        *(.rodata*)
        *(.srodata*)
        __rodata_end = .;
    } > RAM
    
    /* Initialized data */
    .data :
    {
        . = ALIGN(0x1000);
        __data_start = .;
        *(.data*)
        *(.sdata*)
        __data_end = .;
    } > RAM
    
    /* Zero-initialized data */
    .bss :
    {
        . = ALIGN(0x100);
        __bss_start = .;
        *(.bss*)
        *(.sbss*)
        *(COMMON)
        __bss_end = .;
    } > RAM
    
    /* Align and place trampoline section after .bss */
    . = ALIGN(0x400);          /* page-align to 1024 bytes */
    __trampsec_start = .;
    trampsec : ALIGN(0x400)
    {
        KEEP(*(trampsec))      /* Note: no dots - matches ".section trampsec" */
    } > RAM
    __trampsec_end = .;
    
    /* Stack pointer top (safe for QEMU / runtime) */
    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
    
    /* End of program (after trampoline) */
    . = ALIGN(0x10);
    _end = .;
}