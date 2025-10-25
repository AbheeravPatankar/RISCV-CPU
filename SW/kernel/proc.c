
#include "proc.h"

PROC processes[MAX_PROC];

// function to allocate new pid for the process

int alloc_pid(void)
{
    static int pid_counter = -1;   
    pid_counter++;
    return pid_counter;
}

//initialize the process table 
void procinit()
{
    unsigned int kmem_end = KMEM_START + KMEM_SIZE;
    for(int i = 0; i < 10; i++)
    {
        //init Kstack for 10 processes (max proc = 10)
        processes[i].state = UNUSED;
        processes[i].kstatck = (uint32*)(kmem_end - (i + 1) * PAGE_SIZE);
    }
}

//find unused proc struct
PROC* alloc_proc()
{
    for(int i = 0; i < MAX_PROC; i++)
    {
        if(processes[i].state == UNUSED)
        {
            // allocate that structure member and return 
            processes[i].state = USED;
            return &processes[i];
        }
    }
    return NULL;
}

// initialize the first user process (proc which will invoke the shell)
void userinit(void)
{
    int pid = alloc_pid();
    PROC* p = alloc_proc();
    if(p == NULL)
        return ;
    p->pid = pid;
    PAGE* pagetable = create_page_table();
    p->pagetable = (uint32*)(pagetable);
    // map the trapframe and trampoline section
    map_va_to_pa(pagetable, 0 , trampoline , 2);
    PAGE* trapframe = alloc_page();
    map_va_to_pa(pagetable, PAGE_SIZE, (uint32*)trapframe, 3);
    // parse the header and allocate pages 
}

// function to create a page for the user stack and heap 
// function to find a free proc struct for the new process 
// function to create a page table for the process 
// allocproc() - find a free proc struct , allocate pid , create pagetable , map trapframe and trampoline , change state to USED , change the context to resume execution 
// freeproc() - free all pages occupied by the proc , free its trapframe , free the proc structure 
// sleep()
// wait()
// exit()
// yeild()
