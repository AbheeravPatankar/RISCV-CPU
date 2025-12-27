#include "utils.h"

//Write a word at addr
void writei(int val, uint32* addr)
{
    *addr = val;
}

//write the same value from the given start addr to size bytes
//src should be 4 byte aligned
int memstr(char* src, int val, uint32 size)
{
    if((int)src % 4 != 0) // src is not 4 byte aligned 
        return 1;
    for(int i = 0 ; i < size ; i++)
    {
        *src = val;
        src++;
    }
    return 0;
}

// write something to the scratch space
void write_scratch(int val, int offset)
{
    uint32* addr = BAR + offset * 4;
    *addr = val;
}

// copy data from one physical address to other 
void k_memcpy(char* dest, char* src, uint32 size)
{
    for(int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
} 

//get max of 2 nums 
int max(int a, int b)
{
    if( a >= b )
        return a;
    else
        return b;
}

// get min of 2 nums 
int min(int a, int b)
{
    if( a >= b )
        return b;
    else
        return a;
}

// convert a binary number to decimal
int binaryToDecimal(int* arr, int size) 
{
    int decimal = 0;
    for (int i = 0; i < size; i++) 
    {
        decimal = (decimal << 1) | arr[i];  // shift left and add current bit
    }
    return decimal;
}

// copy the chars from src to dest until we hit a '\0' char
void k_strcpy(char* dest, char* src)
{
    uint32 index = 0;
    while(src[index])
    {
        dest[index] = src[index];
        index++;
    }
    // append the NULL char at the end 
    
    dest[index+1] = '\0';
    return ;
}

int strcmp(char* str1, char*str2)
{
    uint32 counter = 0;
    while(str1[counter] != '\0')
    {
        if(str2[counter] == '\0')
            return 0;
        if(str1[counter] != str2[counter])
            return 0;

        counter++;
    }

    if(str2[counter] != '\0')
            return 0;
    else
        return 1;
    
}

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
