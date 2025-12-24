#define TRAPFRAME 4096

.section .trampoline, "ax"
.globl trampoline
.globl uservec
.globl userret

.align 4
trampoline:
uservec:
    csrw sscratch, a0
    li a0, TRAPFRAME

    sw ra, 40(a0)
    sw sp, 48(a0)
    sw gp, 56(a0)
    sw tp, 64(a0)
    sw t0, 72(a0)
    sw t1, 80(a0)
    sw t2, 88(a0)
    sw s0, 96(a0)
    sw s1, 104(a0)
    sw a1, 120(a0)
    sw a2, 128(a0)
    sw a3, 136(a0)
    sw a4, 144(a0)
    sw a5, 152(a0)
    sw a6, 160(a0)
    sw a7, 168(a0)
    sw s2, 176(a0)
    sw s3, 184(a0)
    sw s4, 192(a0)
    sw s5, 200(a0)
    sw s6, 208(a0)
    sw s7, 216(a0)
    sw s8, 224(a0)
    sw s9, 232(a0)
    sw s10, 240(a0)
    sw s11, 248(a0)
    sw t3, 256(a0)
    sw t4, 264(a0)
    sw t5, 272(a0)
    sw t6, 280(a0)

    csrr t0, sscratch
    sw t0, 112(a0)

    lw sp, 8(a0)
    lw tp, 32(a0)
    lw t0, 16(a0)
    lw t1, 0(a0)
    csrw satp, t1
    jr t0

.align 4
userret:
    csrw satp, a0
    li a0, TRAPFRAME

    lw ra, 40(a0)
    lw sp, 48(a0)
    lw gp, 56(a0)
    lw tp, 64(a0)
    lw t0, 72(a0)
    lw t1, 80(a0)
    lw t2, 88(a0)
    lw s0, 96(a0)
    lw s1, 104(a0)
    lw a1, 120(a0)
    lw a2, 128(a0)
    lw a3, 136(a0)
    lw a4, 144(a0)
    lw a5, 152(a0)
    lw a6, 160(a0)
    lw a7, 168(a0)
    lw s2, 176(a0)
    lw s3, 184(a0)
    lw s4, 192(a0)
    lw s5, 200(a0)
    lw s6, 208(a0)
    lw s7, 216(a0)
    lw s8, 224(a0)
    lw s9, 232(a0)
    lw s10, 240(a0)
    lw s11, 248(a0)
    lw t3, 256(a0)
    lw t4, 264(a0)
    lw t5, 272(a0)
    lw t6, 280(a0)

    lw a0, 112(a0)
    sret
