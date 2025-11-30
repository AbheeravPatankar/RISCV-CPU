#include "timer.h"

void handle_mtimer() 
{
    uint32 now = *CLINT_MTIME;
    *CLINT_MTIMECMP = now + 40000000ULL; // next 4 seconds

    asm volatile("csrs mstatus, %0" :: "r"(1 << 3));
}

void timerinit() 
{
    uint32 now = *CLINT_MTIME;

    uint32 tick_hz = 10000000ULL;   // QEMU sets mtime to ~10 MHz
    uint32 interval = 4000ULL * 10000ULL; // 4000 ms = 4 sec → 4 * 10M

    *CLINT_MTIMECMP = now + interval;

    // // Enable machine-timer interrupt
    // asm volatile("csrs mie, %0" :: "r"(0x80));   // MTIE
    // asm volatile("csrs mstatus, %0" :: "r"(0x8)); // MIE
    // asm volatile("csrw mtvec, %0" :: "r"(handle_mtimer));
}





