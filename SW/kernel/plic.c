#include "plic.h"

void plicinit()
{
    // we only have 1 core 
    int hart = 0;

    // set desired IRQ priorities non-zero (otherwise disabled).
    *(uint32*)(PLIC + UART0_IRQ*4) = 1;

    // enable uart interrupts in plic
    *(uint32*)PLIC_SENABLE(hart)= (1 << UART0_IRQ);

    // set this hart's S-mode priority threshold to 0.
    *(uint32*)PLIC_SPRIORITY(hart) = 0;
}

plic_claim(void)
{
  int hart = 0;
  int irq = *(uint32*)PLIC_SCLAIM(hart);
  return irq;
}


void plic_complete(int irq)
{
  int hart = 0;
  *(uint32*)PLIC_SCLAIM(hart) = irq;
}