#include "kernel_vm.h"
#include "utils.h"
#include "paging.h"
#include "utils.h"
uint32* kernel_pagetable;

extern char* trampoline;



void map_kva_to_kpa(uint32 va, uint32 pa)
{
    // given a va extract vpn1 and vpn0 
    uint32 vpn1 = (va >> 22) & 0x3FF;

    // Extract VPN[0] (VA[21:12])
    uint32 vpn0 = (va >> 12) & 0x3FF;

    // check if vpn1 is valid entry
    if(!(kernel_pagetable[vpn1] & 1))
    {
        // make the entry , the internal pagetable entry should have only V permissions 
        uint32* pa = alloc_page();
        uint32 pte = ((uint32)pa >> 12) << 10 | PTE_V;
        kernel_pagetable[vpn1] = pte;
    }

    uint32* leaf_pagetable = (kernel_pagetable[vpn1] >> 10) << 12;

    // check if vpn0 entry is valid the lead entry should have RWXV permissions 
    if(!(leaf_pagetable[vpn0] & 1))
    {
        // no need to allocate page as it is one to one mapping 
        leaf_pagetable[vpn0] = ((pa >> 12) << 10 )| (PTE_R | PTE_W | PTE_X | PTE_V );
    }

    return ;
}


//********** Needs to change this function as the paging scheme is sv32 
void init_kernel_paging()
{ 
    // allocate 2 pages for external kernel pagetable
    kernel_pagetable = alloc_page();

    // map the trampoline page
    map_kva_to_kpa(0x00000000, 0x80003000);
    
    // map the kernel .text and .data sections ... 1 - 1 mapping
    uint32 va = 0x80000000;
    for(uint32 i = 0; i < MAX_PTE; i++)
    {
        map_kva_to_kpa(va, va);
        va += PAGE_SIZE;
    }

    // also map the timer register M_TIME and M_CMP 
    map_kva_to_kpa(0x02004000, 0x02004000);
    map_kva_to_kpa(0x0200BFF8, 0x02004000);

    // write the satp register to enable paging in kernel 
    uint32 root_pa  = (uint32)kernel_pagetable;  // ROOT page table
    uint32 root_ppn = root_pa >> 12;

    uint32 satp = (1U << 31) | root_ppn;   // MODE = Sv32

    uint32 test = sv32_va_to_pa(satp, 0x000000ac);

    asm volatile ("csrw satp, %0" :: "r"(satp));
    asm volatile ("sfence.vma zero, zero");
    
    // Set SUM bit to allow S-mode to access user memory
    uint32 sstatus;
    asm volatile ("csrr %0, sstatus" : "=r"(sstatus));
    sstatus |= (1 << 18);  // Set SUM bit (bit 18)
    asm volatile ("csrw sstatus, %0" :: "r"(sstatus));

}