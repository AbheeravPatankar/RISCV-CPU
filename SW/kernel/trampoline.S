#define TRAPFRAME 4096

.section .trampoline, "ax"
.globl trampoline
.globl uservec
.globl userret

.align 4
trampoline:
uservec:
    csrw sscratch, a0  # copy a0 in sscratch 
    li a0, TRAPFRAME

    sw ra, 20(a0)     # offset 20: ra
    sw sp, 24(a0)    
    sw gp, 28(a0)    
    sw tp, 32(a0)    
    sw t0, 36(a0)    
    sw t1, 40(a0)    
    sw t2, 44(a0)    
    sw s0, 48(a0)    
    sw s1, 52(a0)    
    sw a1, 60(a0)    
    sw a2, 64(a0)    
    sw a3, 68(a0)    
    sw a4, 72(a0)    
    sw a5, 76(a0)    
    sw a6, 80(a0)    
    sw a7, 84(a0)    
    sw s2, 88(a0)    
    sw s3, 92(a0)    
    sw s4, 96(a0)    
    sw s5, 100(a0)   
    sw s6, 104(a0)   
    sw s7, 108(a0)   
    sw s8, 112(a0)   
    sw s9, 116(a0)   
    sw s10, 120(a0)  
    sw s11, 124(a0)  
    sw t3, 128(a0)   
    sw t4, 132(a0)   
    sw t5, 136(a0)   
    sw t6, 140(a0)

    # save user a0 in trapframe (it was loaded into sscratch before )
    csrr t0, sscratch
    sw t0, 56(a0)

    # restore kernel stack , kernel usertrap , and kernel page table  
    lw sp, 0(a0)
    lw t0, 4(a0)
    lw t1, 8(a0)
    lw tp, 16(a0)    
    
    csrw satp, t1
    sfence.vma zero, zero
    jr t0

.align 4
userret:
    csrw satp, a0
    sfence.vma zero, zero
    li a0, TRAPFRAME

    lw ra, 20(a0)     # offset 20: ra
    lw sp, 24(a0)    
    lw gp, 28(a0)    
    lw tp, 32(a0)    
    lw t0, 36(a0)    
    lw t1, 40(a0)    
    lw t2, 44(a0)    
    lw s0, 48(a0)    
    lw s1, 52(a0)    
    lw a1, 60(a0)    
    lw a2, 64(a0)    
    lw a3, 68(a0)    
    lw a4, 72(a0)    
    lw a5, 76(a0)    
    lw a6, 80(a0)    
    lw a7, 84(a0)    
    lw s2, 88(a0)    
    lw s3, 92(a0)    
    lw s4, 96(a0)    
    lw s5, 100(a0)   
    lw s6, 104(a0)   
    lw s7, 108(a0)   
    lw s8, 112(a0)   
    lw s9, 116(a0)   
    lw s10, 120(a0)  
    lw s11, 124(a0)  
    lw t3, 128(a0)   
    lw t4, 132(a0)   
    lw t5, 136(a0)   
    lw t6, 140(a0)
      
    lw a0, 56(a0)    
    sret
