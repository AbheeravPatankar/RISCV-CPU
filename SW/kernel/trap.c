#include "trap.h"
#include "proc.h"
#include "paging.h"
#include "timer.h"
#include "syscall.h"


// Needs to set these addresses 
char* trampoline = 0x00000000;
char* userret = 0x000000b0;
char* uservec = 0x0000000;

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

    if (is_first) 
    {
        is_first = 0;
        kexec("init");
    }

    uint32 x;
    asm volatile ("csrr %0, sstatus" : "=r"(x));
    x &= ~(1 << 8);   // clear SPP (return to U-mode)
    x |=  (1 << 5);   // set SPIE
    asm volatile ("csrw sstatus, %0" :: "r"(x));

    /* Set sepc to user instruction pointer */
    asm volatile ("csrw sepc, %0" :: "r"(p->ptr_to_trapframe->epc));

}

// function to process supervisor interrupts 
// we jump here from trampoline 
void usertrap()
{

    // set stvec to kernelvec for kernel traps 

    // kernel traps not implemented by now 

    // save the sepc 
    PROC* p = myproc();
    uint32 sepc;
    asm volatile("csrr %0, sepc" : "=r" (sepc));
    p->ptr_to_trapframe->epc = sepc;

    // // read scause 

    uint32 scause;
    asm volatile("csrr %0, scause" : "=r" (scause));

    if(scause == 0x80000001)
    {
        // timer interrupt
        timer_interrupt();
        kyeild();
        
    }
    else if(scause == 0x8)
    {
        // its a syscall......
        // check if the process is killed ? 

        if(killed(p))
        {
            kexit(-1);
        }
        // TODO : turn on interrupts .... will enable this in a later phase 

        // read the a7 register to identify which system call is this

        // increment the epc by 4 


        int sys_call_id = p->ptr_to_trapframe->a7;
        // call the appropriate syscall handler
        p->ptr_to_trapframe->epc += 4;

        switch(sys_call_id)
        {
            case SYS_fork:
                kfork();
                break;

            case SYS_exit:
                kexit(p->ptr_to_trapframe->a0);
                break;

            default:
                break;           

        }

       

    }

   fork_ret();  
}

