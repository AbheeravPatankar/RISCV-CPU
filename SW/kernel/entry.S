    .section .text.entry
    .global _start
_start:
    li      sp, 0x80011000

    # Call main
    call    start

    # Infinite loop after main returns
1:
    j       1b
