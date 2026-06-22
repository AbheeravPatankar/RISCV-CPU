#include "trap.h"
#include "proc.h"
#include "paging.h"
#include "timer.h"
#include "syscall.h"
#include "plic.h"
#include "uart.h"

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
    asm volatile("csrw stvec, %0" :: "r" ((uint32)kernelvec));

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
        char buff[MAX_PROC_NAME];
        uint32 sys_arg;
        uint32* sys_arg_vec;
        if(killed(p))
        {
            kexit(-1);
        }

        // TODO : turn on interrupts .... will enable this in a later phase
        asm volatile ("csrs sstatus, %0" :: "r"(1 << 1)); 

        // read the a7 register to identify which system call is this
        int sys_call_id = p->ptr_to_trapframe->a7;
        // increment the epc by 4 
        p->ptr_to_trapframe->epc += 4;

        // call the appropriate syscall handler


        switch(sys_call_id)
        {
            case SYS_fork:
                p->ptr_to_trapframe->a0 = kfork();
                break;

            case SYS_exit:
                sys_arg = p->ptr_to_trapframe->a0;
                kexit(sys_arg);
                break;

            case SYS_exec:
                sys_arg = p->ptr_to_trapframe->a0;
                copyin(p->pagetable , (uint32*)buff, sys_arg, MAX_PROC_NAME);
                kexec(buff);
                break;

            case SYS_wait:
            // only the parent invokes wait so it will sleep on its own address 
                kwait(p);  
                break;

            case SYS_sbrk:
                sys_arg = p->ptr_to_trapframe->a0;
                p->ptr_to_trapframe->a0 = alloc_mem(sys_arg);
                break;

            case SYS_read:
                // args will be char* buff and uint32 size
                sys_arg_vec = p->ptr_to_trapframe->a0;      // get the ptr to buff 
                sys_arg = p->ptr_to_trapframe->a1;          // get the size of the date to read 
                console_read((char*)sys_arg_vec, (uint32)sys_arg);
                break;
            case SYS_write:
                // args will be char* buff and uint32 size
                sys_arg_vec = p->ptr_to_trapframe->a0;      // get the ptr to buff 
                sys_arg = p->ptr_to_trapframe->a1;          // get the size of the date to read
                console_write((char*)sys_arg_vec, (uint32)sys_arg);
                break;       
            default:
                break;           

        }
    }
    else if(scause == 0x80000009)
    {
        
        // claim that device interrupt from plic
        int irq = plic_claim();

        // device interrupt (for now there are no external devices other than uart device )

        // handle the uart interrupt
        uartintr();

        if(irq)
            plic_complete(irq);
    }

   fork_ret();  
}


void kerneltrap()
{

    // read sepc and scause 
    uint32 sepc;
    asm volatile("csrr %0, sepc" : "=r" (sepc));

    uint32 scause;
    asm volatile("csrr %0, scause" : "=r" (scause));

    uint32 sstatus;
    asm volatile("csrr %0, sstatus" : "=r" (sstatus));

    if(scause == 0x80000001)
    {
        // timer interrupt
        timer_interrupt();

        kyeild();
        
    }

    else if(scause == 0x80000009)
    {
        
        // claim that device interrupt from plic
        int irq = plic_claim();

        // device interrupt (for now there are no external devices other than uart device )

        // handle the uart interrupt
        uartintr();

        if(irq)
            plic_complete(irq);
    }

    // restore sepc and sstatus value  
    asm volatile("csrw sepc, %0" : : "r" (sepc));
    asm volatile("csrw sstatus, %0" : : "r" (sstatus));
}