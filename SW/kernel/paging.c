#include "paging.h"
#include "utils.h"
struct page_list
{
    PAGE* free_page;
}umem;


int page_free(PAGE*  page_addr )
{
    // check if page address is valid 
    if( (int)page_addr % PAGE_SIZE != 0 || (int)page_addr < UMEM_START || (int)page_addr > UMEM_END)
    {
        // incorrect page address
        return 1;
    }

    //else scrub the page and attach to page_list
    memstr((int*)page_addr, 0 , PAGE_SIZE);
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

// take the va and size,  and allocate pages and create page table entries .
// Given va convert to pa and return pa
// free a given page table ( entries and pages ) 
