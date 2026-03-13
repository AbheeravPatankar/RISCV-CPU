#include "paging.h"
#include "proc.h"
#include "timer.h"
#include "kernel_vm.h"
#include "plic.h"
#include "uart.h"


int main(void)
{
    paginginit();           // link all the free pages in the user space to the page_list
    init_kernel_paging();   // initialize the kernel page table
    plicinit();
    uartinit();
    procinit();             // initialize the array of proc structures 
    userinit();             // create the first user process and party starts !!!
    scheduler();            // schedule a user process for execution 
    return 0;
}

