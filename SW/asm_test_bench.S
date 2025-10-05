.section .text
    .globl _start

_start:
    # Store 25 at address 3072 (0xC00)
    li   t0, 3072        # base address
    li   t1, 25
    sw   t1, 0(t0)       # Mem[0xC00] = 25

    # Store 26 at address 3076 (0xC04)
    li   t0, 3076
    li   t1, 26
    sw   t1, 0(t0)       # Mem[0xC04] = 26

    # Store 'a' (ASCII 97) at address 3081 (0xC09)
    li   t0, 3081
    li   t1, 'a'
    sb   t1, 0(t0)       # Mem[0xC09] = 97

    # Store 'b' (ASCII 98) at address 3082 (0xC0A)
    li   t0, 3082
    li   t1, 'b'
    sb   t1, 0(t0)       # Mem[0xC0A] = 98

