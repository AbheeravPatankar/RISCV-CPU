.globl timervec
timervec:
    la t0, timer_scratch     # load pointer to scratch
    # Save registers we will clobber.
    sw ra, 0(t0)
    sw t1, 4(t0)
    sw t2, 8(t0)
      
    
    # Load MTIMECMP address (scratch[3]) and interval (scratch[4])
    la t0, timer_scratch     # reload scratch pointer
    lw t1, 12(t0)            # t1 = scratch[3] = MTIMECMP address
    lw t2, 16(t0)            # t2 = scratch[4] = interval
    
    # Set next timer interrupt: MTIMECMP += interval
    lw t3, 0(t1)             # t3 = MTIMECMP current value
    add t3, t3, t2           # t3 = t3 + interval
    sw t3, 0(t1)             # write back to MTIMECMP
    
   
    li a1, 2
    csrw sip, a1

    # Restore registers
    la t0, timer_scratch
    lw ra, 0(t0)
    lw t1, 4(t0)
    lw t2, 8(t0)

    mret                     # return from machine-mode interrupt