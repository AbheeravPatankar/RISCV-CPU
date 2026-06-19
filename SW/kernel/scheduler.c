#include "scheduler.h"

extern PROC processes[MAX_PROC];
extern CPU cpu;
// function to schedule a process ( runs on the cpu stack ) 
void scheduler()
{
    do
    {
        int found = 0;
        for(int i = 0; i < MAX_PROC; i++)
        {
            // find a process which can be scheduled 
            if(processes[i].state == RUNNABLE)
            {
                // disable interrupts again before scheduling a new process 
                asm volatile ("csrc sstatus, %0" :: "r"(1 << 1));
                
                // make the context switch
                processes[i].state = RUNNING;
                cpu.current_proc = processes + i ;
                swtch(&cpu.context, &processes[i].context);

                cpu.current_proc = NULL;
                found = 1;
            } 
        }
        if(found == 0)
        {
            asm volatile ("csrs sstatus, %0" :: "r"(1 << 1)); 
        }   
    }while(1);
}
