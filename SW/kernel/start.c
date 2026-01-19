#include "utils.h"
#include "trap.h"
#include "timer.h"
int main(void);


static inline void write_csr_medeleg(uint32 val) 
{
    asm volatile("csrw medeleg, %0" :: "r"(val));
}

static inline void write_csr_mideleg(uint32 val) 
{
    asm volatile("csrw mideleg, %0" :: "r"(val));
}

static inline uint32 read_csr_sie(void) 
{
    uint32 val;
    asm volatile("csrr %0, sie" : "=r"(val));
    return val;
}

static inline void write_csr_sie(uint32 val) 
{
    asm volatile("csrw sie, %0" :: "r"(val));
}

#define SIE_SEIE (1 << 9)   // Supervisor external interrupt enable
#define SIE_STIE (1 << 5)   // Supervisor timer interrupt enable
#define SIE_SSIE (1 << 1)   // Supervisor software interrupts enable 


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
    // delegate all the interrupts and exceptions from M mode to S mode 
    write_csr_medeleg(0xffff);

    // Write 0xffff to mideleg
    write_csr_mideleg(0xffff);

    // Read sie, OR with SEIE and STIE, write back
    uint32 sie = read_csr_sie();
    sie |= (SIE_SEIE | SIE_STIE | SIE_SSIE);
    write_csr_sie(sie);


    timerinit();            // init the timer for interrupts in M mode 

    // Store hart id in tp (x4) Dont know why ??

    // call mret ( jump to main )
    asm volatile ("mret");

}