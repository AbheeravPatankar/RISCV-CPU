# Context switch
#
#   void swtch(struct context *old, struct context *new);
# 
# Save current registers in old. Load from new.	


.globl swtch
swtch:
        sw ra, 0(a0)
        sw sp, 8(a0)
        sw s0, 16(a0)
        sw s1, 24(a0)
        sw s2, 32(a0)
        sw s3, 40(a0)
        sw s4, 48(a0)
        sw s5, 56(a0)
        sw s6, 64(a0)
        sw s7, 72(a0)
        sw s8, 80(a0)
        sw s9, 88(a0)
        sw s10, 96(a0)
        sw s11, 104(a0)

        lw ra, 0(a1)
        lw sp, 8(a1)
        lw s0, 16(a1)
        lw s1, 24(a1)
        lw s2, 32(a1)
        lw s3, 40(a1)
        lw s4, 48(a1)
        lw s5, 56(a1)
        lw s6, 64(a1)
        lw s7, 72(a1)
        lw s8, 80(a1)
        lw s9, 88(a1)
        lw s10, 96(a1)
        lw s11, 104(a1)
        
        ret

	
