#include "trap.h"
#include "proc.h"
#include "paging.h"

extern char trampoline[], uservec[];

void prepare_return(void)
{
    static uint32 is_first = 1;

    /* Disable supervisor interrupts (SIE) */
    asm volatile ("csrc sstatus, %0" :: "r"(1 << 1));  // SSTATUS_SIE

    /* Set stvec back to user trap vector */
    asm volatile ("csrw stvec, %0" :: "r"(uservec));

    /* Reset process kernel stack */
    PROC *p = myproc();
    p->ptr_to_trapframe->kernel_sp = p->kstack + PAGE_SIZE;

    if (is_first) {
        is_first = 0;
        exec("init");
    }

    /* 
     * Set:
     *  - SPP = 0 (return to U-mode)
     *  - SPIE = 1 (enable interrupts after sret)
     */
    uint32 x;
    asm volatile ("csrr %0, sstatus" : "=r"(x));
    x &= ~(1 << 8);   // clear SPP (return to U-mode)
    x |=  (1 << 5);   // set SPIE
    asm volatile ("csrw sstatus, %0" :: "r"(x));

    /* Set sepc to user instruction pointer */
    asm volatile ("csrw sepc, %0" :: "r"(p->ptr_to_trapframe->epc));

}

