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
            // sleep until a new process is changed into runnable state 
        }        
    }while(1);
}
