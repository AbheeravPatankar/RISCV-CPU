#include "paging.h"

struct page_list
{
    PAGE* free_page;
}umem;


int page_free(PAGE* page_addr)
{
    // check if page address is valid 
    if( (int)page_addr % PAGE_SIZE != 0 || (int)page_addr < UMEM_START || (int)page_addr > UMEM_END)
    {
        // incorrect page address
        return 1;
    }

    //else scrub the page and attach to page_list
    memstr((uint32*)page_addr, 0 , PAGE_SIZE);
    PAGE* temp = umem.free_page;
    umem.free_page = page_addr;

    page_addr->ptr_to_page = temp;
    return 0;
}

//free all the pages
void initpaging()
{
    for(unsigned int i = UMEM_START; i < UMEM_END; i+= PAGE_SIZE)
    {
        page_free((PAGE* ) i);
    }
}

// allocate a free page
PAGE* alloc_page()
{
    PAGE* free_page = umem.free_page;
    PAGE* temp = umem.free_page->ptr_to_page;
    umem.free_page = temp;
    return free_page;
}
// function to create a page table

PAGE* create_page_table()
{
    PAGE* ptr_to_page = alloc_page();
    return ptr_to_page;
}

// take the va and size, and allocate pages and create page table entries .
// Basically create page table for a certain process 
void map_vm(uint32* pagetable, uint32 va, uint32 size, int perms )
{
    PAGE* pa = NULL;
    int pt_index = 0;
    if(size < 0 || va % PAGE_SIZE != 0)
    {
        return ;
    }
    while(size > PAGE_SIZE)
    {
        pa = alloc_page(); // page is the pa 
        pt_index = va / PAGE_SIZE;
        pagetable[pt_index] = (uint32)pa + perms ;
        //scrub the page 
        memstr((uint32*)pa, 0, PAGE_SIZE);
        va += PAGE_SIZE;
        size -= PAGE_SIZE;
    }
}

// Given va convert to pa and return pa
uint32* va_to_pa(uint32* pagetable, uint32 va)
{
    int pt_index = va / PAGE_SIZE;
    uint32 pa = pagetable[pt_index];
    uint32 offset = va % PAGE_SIZE;
    return (uint32*)(pa + offset);    
} 

// function to copy data from kernel space to user space
void copyout(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size)
{
    while( size > 0 )
    {
        uint32* u_pa = va_to_pa(pagetable, u_va);
        int size_temp = max(PAGE_SIZE - ((uint32)u_pa % PAGE_SIZE) , size);
        memcpy(u_pa, k_pa, size_temp);
        u_va += size_temp;
        k_pa += size_temp;
        size -= size_temp;
    }
}
// function to copy data from user space to kernel space 

void copyin(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size)
{
    while( size > 0 )
    {
        uint32* u_pa = va_to_pa(pagetable, u_va);
        int size_temp = max(PAGE_SIZE - ((uint32)u_pa % PAGE_SIZE) , size);
        memcpy(k_pa, u_pa, size_temp);
        u_va += size_temp;
        k_pa += size_temp;
        size -= size_temp;
    }
}

// free a given page table ( entries and pages ) 
void unmap_vm(uint32* pagetable)
{
    for(int i = 0 ; i < PAGE_SIZE / 4; i++)
    {
        if(pagetable[i] % 2 == 1)
        {
            // if entry is valid call free on the page
            uint32* page_addr = (uint32*) ((uint32)pagetable[i] / PAGE_SIZE );
            page_free((PAGE*)page_addr);
        }
           
    }
}

// map a given va to a given pa 
void map_va_to_pa(uint32* pagetable, uint32 va, uint32* pa, int perms)
{
    // check if both va and pa are page aligned 
    if(va % PAGE_SIZE != 0)
    {
        return ;
    }
    if( (uint32)pa % PAGE_SIZE != 0)
    {
        return ;
    }
    int pt_index = va / PAGE_SIZE;
    pagetable[pt_index] = (uint32)pa + perms;
}