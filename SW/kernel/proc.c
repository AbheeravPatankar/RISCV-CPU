
#include "proc.h"
#include "paging.h"
PROC processes[MAX_PROC] ;

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
        processes[i].kstatck = (int*)(kmem_end - (i + 1) * PAGE_SIZE);
    }
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
