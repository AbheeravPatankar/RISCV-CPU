#include "utils.h"
#define PAGE_SIZE  1024
 // For QEMU
#define UMEM_START 0x80100000   
#define UMEM_END   0x80200000
// For my hardware
/*
#define UMEM_START 2048    
#define UMEM_END   8192
*/
typedef struct page
{
    struct page* ptr_to_page;
}PAGE;


int page_free(PAGE* addr);

void initpaging();

PAGE* alloc_page();

PAGE* create_page_table();

void map_vm(uint32* pagetable, uint32 va, uint32 size, int perms );

uint32* va_to_pa(uint32* pagetable, uint32 va);

void copyout(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size);

void copyin(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size);

void unmap_vm(uint32* pagetable);