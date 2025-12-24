#include "utils.h"
#include "trap.h"

int main(void);


void start()
{
    // Set prev privilege mode to supervisor 
    uint32 x;
    asm volatile ("csrr %0, mstatus" : "=r"(x));
    x &= ~MSTATUS_MPP;
    x |= (1 << 11);
    asm volatile ("csrw mstatus, %0" :: "r"(x));
    // Set mepc to main 

    asm volatile ("csrw mepc, %0" :: "r"(main));
    // enable supervisor mode interrupts

    // Do this only on qemu the actual hardware does not have a MMU yet 
    // ---------------------------------------------

    asm volatile ("csrw pmpaddr0, %0" :: "r"(0xFFFFFFFF));
    asm volatile ("csrw pmpcfg0,  %0" :: "r"(0x0F));

    // ---------------------------------------------
    // timerinit();

    // Store hart id in tp (x4) Dont know why ??

    // call mret ( jump to main )
    asm volatile ("mret");

}