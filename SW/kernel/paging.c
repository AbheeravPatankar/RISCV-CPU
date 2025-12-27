#include "paging.h"
#include "trap.h"

extern char* trampoline ;

struct page_list
{
    PAGE* free_page;
}umem;

int set_perms(char* code)
{

    int perms_bin = 0;
    for(int i = 0 ; i < 5; i++)
    {
        if (code[i] == 'U') 
        {
            perms_bin += 16;
        }
        else if (code[i] == 'X') 
        {
            perms_bin += 8;
        }
        else if (code[i] == 'W') 
        {
            perms_bin += 4;
        }
        else if (code[i] == 'R') 
        {
            perms_bin += 2;
        }
        else if (code[i] == 'V') 
        {
            perms_bin += 1;
        }
        else
        {
            break;
        }   
    }
    return perms_bin;
}

// checks if the page table entry is valid  
int is_pte_valid(uint32 pte)
{
    return pte & 1;
}

int page_free(PAGE* page_addr)
{
    // check if page address is valid 
    if( (int)page_addr % PAGE_SIZE != 0 || (int)page_addr < UMEM_START || (int)page_addr > UMEM_END)
    {
        // incorrect page address
        return 1;
    }

    // else scrub the page and attach to page_list
    memstr((uint32*)page_addr, 0 , PAGE_SIZE);
    PAGE* temp = umem.free_page;
    umem.free_page = page_addr;

    page_addr->ptr_to_page = temp;
    return 0;
}

// free all the pages
void paginginit()
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

// gets the internal page number (top-level index: VA[31:22])
uint32 get_internal_page_no(uint32 va)
{
    return (va >> 22) & 0x3FF;
}

// gets the external page number (second-level index: VA[21:12])
uint32 get_leaf_page_no(uint32 va)
{
    return (va >> 12) & 0x3FF;
}

// extract the pa from a pte   * this also works as page round down function 
uint32 extract_pa_from_pte(uint32 pte)  
{
    // Zero out the lowest 12 bits (mask with ~0xFFF)
    return (pte >> 10) << 12;
}

// extract only the perms from the pte
uint32 extract_perms_from_pte(uint32 pte)
{
    // Zero out the lowest 12 bits (mask with ~0xFFF)
    return pte & 0x3FF;
}

// Given va check if the page is allocated for the va if not allocate pages and ptes in both internal and leaf pagetables  
uint32 va_to_pte(uint32* pagetable, uint32 va)
{
    uint32 internal_page_no = get_internal_page_no(va);
    uint32 internal_pte = pagetable[internal_page_no];

    // check if internal pte
    uint32* leaf_pagetable;
    if(!is_pte_valid(internal_pte))
    {
        uint32* pa = alloc_page();
        memstr((char*)pa, 0, PAGE_SIZE);
        pagetable[internal_page_no] = (((uint32)pa >> 12) << 10) | (set_perms("UV"));
        leaf_pagetable = pa;

    }
    else
    {
        leaf_pagetable = extract_pa_from_pte(internal_pte);
    } 
    uint32 leaf_page_no = get_leaf_page_no(va);
    uint32 leaf_pte =  leaf_pagetable[leaf_page_no];

    // check if the leaf pte is valid
    uint32 pte ;
    if(!is_pte_valid(leaf_pte))
    {
        uint32* pa = alloc_page();
        memstr((char*)pa, 0, PAGE_SIZE);
        leaf_pagetable[leaf_page_no] = (((uint32)pa >> 12) << 10) | (set_perms("RWXUV"));
        uint32 pte = leaf_pagetable[leaf_page_no];
    }
    return leaf_pagetable[leaf_page_no];
} 

// takes a page aligned virtual address and size of memory and allocates contiguous physical pages  
void map_vm(uint32* pagetable, uint32 va, int size, int perms )
{
    // check if the virtual address is non negative and page aligned 
    PAGE* pa = NULL;
    int pt_index = 0;
    if(size < 0 || va % PAGE_SIZE != 0)
    {
        return ;
    }
    do
    {
        // check if page is already allocated and allocate pages and ptes if not already 
        va_to_pte(pagetable , va);        
        va += PAGE_SIZE;
        size -= PAGE_SIZE;
           
    }while(size > 0);
}

// function to copy data from kernel space to user space
void copyout(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size)
{
    while( size > 0 )
    {
        uint32 pte = va_to_pte(pagetable, u_va);
        uint32 u_pa = extract_pa_from_pte(pte);
        int size_temp = min(PAGE_SIZE - ((uint32)u_pa % PAGE_SIZE) , size);
        uint32 offset = u_va & 0xFFF;
        u_pa = u_pa + offset;
        k_memcpy((char*)u_pa, (char*)k_pa, size_temp);
        u_va = (char*)u_va + size_temp;
        k_pa =(char*)k_pa + size_temp;
        size -= size_temp;
    }
}

// function to copy data from user space to kernel space 
void copyin(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size)
{
    while( size > 0 )
    {
        uint32 pte = va_to_pte(pagetable, u_va);
        uint32 u_pa = extract_pa_from_pte(pte);
        uint32 offset = u_va & 0xFFF;
        int size_temp = min(PAGE_SIZE - ((uint32)u_pa % PAGE_SIZE) , size);
        u_pa = u_pa + offset;
        k_memcpy((char*)k_pa, (char*) u_pa, size_temp);
        u_va += size_temp;
        k_pa += size_temp;
        size -= size_temp;
    }
}

// free a given page table ( entries and pages )
void unmap_vm(uint32* pagetable)
{
    for(int i = 0 ; i < MAX_PTE; i++)
    {    
        if(is_pte_valid(pagetable[i]))
        {   
            uint32* leaf_pagetable = extract_pa_from_pte( pagetable[i] );
            for(int j = 0 ; j < MAX_PTE; j++)
            {
                if(is_pte_valid(leaf_pagetable[j]))
                {
                    page_free(extract_pa_from_pte(leaf_pagetable[j]));
                }
            }
            page_free(extract_pa_from_pte(leaf_pagetable));
        }    
    }
    page_free(pagetable);
}

// make an entry in the pagetable given va find the pt index and insert the pa and the perms 
void map_va_to_pa(uint32* pagetable, uint32 va, uint32 pa, int perms)
{
    // check and allocate space for the va
    uint32 vpn1 = get_internal_page_no(va);
    uint32 vpn0 = get_leaf_page_no(va);

    uint32* internal_pagetable =  extract_pa_from_pte( pagetable[vpn1] );
    internal_pagetable[vpn0] = (((uint32)pa >> 12) << 10) | perms ;
    return ;
}   

// copy entire pagetable and the pages allocated to the process
void copy_proc_mem(uint32* parent_pagetable, uint32* child_pagetable)
{
    // iterate over all the entries in the parent pagetable 
    for(int i = 1; i < MAX_PTE; i++)
    {
        // check if this entry is valid 
        if(is_pte_valid(parent_pagetable[i]))
        {
            // load the lead pagetable
            uint32* leaf_pagetable = extract_pa_from_pte(parent_pagetable[i]);
            for(int j = 0; j < MAX_PTE; j++)
            {
                // check if the lead entry is valid
                if(is_pte_valid(leaf_pagetable[j]))
                {
                    // actually copy the mem contents 
                    uint32 va = (i << (22)) | (j << 12);
                    map_vm(child_pagetable, va,PAGE_SIZE, extract_perms_from_pte(leaf_pagetable[j]));
                    uint32 child_pa = va_to_pte(child_pagetable, va);
                    child_pa = extract_pa_from_pte(child_pa);
                    k_memcpy(child_pa, extract_pa_from_pte(parent_pagetable[j]), PAGE_SIZE);
                }
            }

        }
    }
}

// map the trampoline section to the user page table
void map_trampoline_and_trapframe(uint32* pagetable, uint32* trapframe)
{
    // map the trampoline page at va = 0x0000
    map_vm(pagetable,0x000, PAGE_SIZE, set_perms("UXV"));
    map_va_to_pa(pagetable, 0x000, 0x80003000, set_perms("RWXUV"));

    // map the trapframe page
    map_vm(pagetable,PAGE_SIZE, PAGE_SIZE, set_perms("RWUV"));
    map_va_to_pa(pagetable, PAGE_SIZE, trapframe , set_perms("RWUV")); 
    return ;
}