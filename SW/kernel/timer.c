# include "timer.h"

# define CLINT      0x02000000
# define MTIMECMP   0x02004000
# define MTIME      0x0200BFF8

uint32 timer_scratch[5];


void timerinit(void)
{
    uint32 interval = 1000000;

    // set the MTIMECMP to current time plus interval 
    *((uint32*)MTIMECMP) = *((uint32*)MTIME) + interval ;

    timer_scratch[3] = (uint32)MTIMECMP;
    timer_scratch[4] = interval;

    // write to mtvec address of timervec 
    asm volatile("csrw mtvec, %0" :: "r"(timervec));

    // enable machine mode timer interrupts here 
    // basically set mie and mtie 

    asm volatile("csrs mie, %0" :: "r"(1 << 7));
    asm volatile("csrs mstatus, %0" :: "r"(1 << 3));

}






