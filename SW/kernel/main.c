#include "paging.h"
#include "proc.h"
#include "timer.h"

int main(void)
{
    paginginit();       // link all the free pages in the user space to the page_list 
    procinit();         // initialize the array of proc structures 
    timerinit();       // init the timer for interrupts in M mode 
    userinit();         // create the first user process and party starts !!!
    scheduler();        // schedule a user process for execution 
    return 0;
}
