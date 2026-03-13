#include "utils.h"


#define PLIC 0x0c000000                                                
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)



#define UART0_IRQ 10 // --> this is the source ID for UART interrupts 


void plicinit();

int plic_claim();

void plic_complete(int irq);