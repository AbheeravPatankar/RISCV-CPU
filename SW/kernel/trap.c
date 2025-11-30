#include "trap.h"

void prepare_return()
{
    // disable interrupts 
    
    // set the stvec back to uservec 

    // reset the processes's kernel stack 

    // set the spie and spp registers ( SIE is to determine if interrupts were enabled in the preivious priv mode but not sure why SPP is used )

    // write sepc as trapframe->epc 

}
