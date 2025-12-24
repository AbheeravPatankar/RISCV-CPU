#include "kernel_vm.h"
#include "utils.h"
#include "paging.h"

uint32* kernel_pagetable;
char __trampoline_lma[];


typedef uint32 pte_t;
#define PTE_V   (1 << 0)
#define PTE_R   (1 << 1)
#define PTE_W   (1 << 2)
#define PTE_X   (1 << 3)
// ----------------------------------------------------------------------------------------------------
//  This function is present only for debugging purposes will exclude or comment while building the real driver 
uint32 sv32_va_to_pa(uint32 satp, uint32 va)
{
    // 1. Check paging mode (bit 31)
    if ((satp & 0x80000000) == 0) 
    {
        // Paging OFF → VA == PA
        return va;
    }

    // 2. Extract root page table physical address
    uint32 root_ppn = satp & 0x3FFFFF;   // bits [21:0]
    uint32 root_pa  = root_ppn << 12;

    // 3. Extract VPNs and offset
    uint32 vpn1   = (va >> 22) & 0x3FF;
    uint32 vpn0   = (va >> 12) & 0x3FF;
    uint32 offset = va & 0xFFF;

    // 4. Level-1 page table lookup
    pte_t *l1_pt = (pte_t *)root_pa;
    pte_t pte1   = l1_pt[vpn1];

    if ((pte1 & PTE_V) == 0) 
    {
        return 0xFFFFFFFF;   // page fault
    }

    // 5. If this is a leaf PTE (superpage)
    if (pte1 & (PTE_R | PTE_W | PTE_X)) 
    {
        uint32 ppn = (pte1 >> 10);
        return (ppn << 12) | (va & 0x3FFFFF);
    }

    // 6. Level-0 page table lookup
    uint32 l0_pa = (pte1 >> 10) << 12;
    pte_t *l0_pt   = (pte_t *)l0_pa;
    pte_t pte0     = l0_pt[vpn0];

    if ((pte0 & PTE_V) == 0) 
    {
        return 0xFFFFFFFF;   // page fault
    }

    // 7. Leaf PTE
    if ((pte0 & (PTE_R | PTE_W | PTE_X)) == 0) 
    {
        return 0xFFFFFFFF;   // invalid leaf
    }

    // 8. Construct final physical address
    uint32 ppn = pte0 >> 10;
    uint32 pa = (ppn << 12) | offset;
    return pa ;
}

// ----------------------------------------------------------------------------------------------------


void map_kva_to_kpa(uint32 va)
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
        leaf_pagetable[vpn0] = ((va >> 12) << 10 )| (PTE_R | PTE_W | PTE_X | PTE_V );
    }

    return ;
}


//********** Needs to change this function as the paging scheme is sv32 
void init_kernel_paging()
{ 
    // allocate 2 pages for external kernel pagetable
    kernel_pagetable = alloc_page();

    // map the trampoline page
    uint32* leaf_pagetable = alloc_page();
    kernel_pagetable[0] = leaf_pagetable + set_perms("V");
    leaf_pagetable[0] = (uint32)__trampoline_lma + set_perms("XV");


    // map the kernel .text and .data sections ... 1 - 1 mapping
    uint32 va = 0x80000000;
    for(uint32 i = 0; i < MAX_PTE; i++)
    {
        map_kva_to_kpa(va);
        va += PAGE_SIZE;
    }

    // write the satp register to enable paging in kernel 
    uint32 root_pa  = (uint32)kernel_pagetable;  // ROOT page table
    uint32 root_ppn = root_pa >> 12;

    uint32 satp = (1U << 31) | root_ppn;   // MODE = Sv32

    sv32_va_to_pa(satp, 0x87ffffb0);

    asm volatile ("csrw satp, %0" :: "r"(satp));
    asm volatile ("sfence.vma zero, zero");

}