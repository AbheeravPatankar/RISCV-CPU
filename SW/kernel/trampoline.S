#define TRAPFRAME 0x10000000

.section trampsec
.globl trampoline
.globl usertrap
trampoline:
.align 4
.globl uservec
uservec:    
	#
        # trap.c sets stvec to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        #

        # save user a0 in sscratch so
        # a0 can be used to get at TRAPFRAME.
        csrw sscratch, a0

        # each process has a separate p->trapframe memory area,
        # but it's mapped to the same virtual address
        # (TRAPFRAME) in every process's user page table.
        li a0, TRAPFRAME
        
        # save the user registers in TRAPFRAME
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

	# save the user a0 in p->trapframe->a0
        csrr t0, sscratch
        sw t0, 112(a0)

        # initialize kernel stack pointer, from p->trapframe->kernel_sp
        lw sp, 8(a0)

        # make tp hold the current hartid, from p->trapframe->kernel_hartid
        lw tp, 32(a0)

        # load the address of usertrap(), from p->trapframe->kernel_trap
        lw t0, 16(a0)

        # fetch the kernel page table address, from p->trapframe->kernel_satp.
        lw t1, 0(a0)


        # install the kernel page table.
        csrw satp, t1

        # jump to usertrap(), which does not return
        jr t0

.globl userret
userret:
        # userret(pagetable)
        # called by usertrapret() in trap.c to
        # switch from kernel to user.
        # a0: user page table, for satp.

        # switch to the user page table.
        sfence.vma zero, zero
        csrw satp, a0
        sfence.vma zero, zero

        li a0, TRAPFRAME

        # restore all but a0 from TRAPFRAME
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

	# restore user a0
        lw a0, 112(a0)
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
