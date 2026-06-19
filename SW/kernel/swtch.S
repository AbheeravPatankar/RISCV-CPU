# Context switch
#
#   void swtch(struct context *old, struct context *new);
# 
# Save current registers in old. Load from new.	

.align 4
.globl swtch
swtch:
        sw ra, 0(a0)
        sw sp, 4(a0)
        sw s0, 8(a0)
        sw s1, 16(a0)
        sw s2, 20(a0)
        sw s3, 24(a0)
        sw s4, 28(a0)
        sw s5, 32(a0)
        sw s6, 36(a0)
        sw s7, 40(a0)
        sw s8, 44(a0)
        sw s9, 48(a0)
        sw s10, 52(a0)
        sw s11, 56(a0)

        lw ra,  0(a1)
        lw sp,  4(a1)
        lw s0,  8(a1)
        lw s1, 16(a1)
        lw s2, 20(a1)
        lw s3, 24(a1)
        lw s4, 28(a1)
        lw s5, 32(a1)
        lw s6, 36(a1)
        lw s7, 40(a1)
        lw s8, 44(a1)
        lw s9, 48(a1)
        lw s10,52(a1)
        lw s11,56(a1)
        
        ret

	
