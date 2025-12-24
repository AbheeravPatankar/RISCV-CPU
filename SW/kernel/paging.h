#pragma once

#include "utils.h"

#define PAGE_SIZE  4096
#define MAX_PTE    1024

 // For QEMU
#define UMEM_START 0x80005000   
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

void paginginit();

PAGE* alloc_page();

PAGE* create_page_table();

void map_vm(uint32* pagetable, uint32 va, int size, int perms );

uint32* va_to_pte(uint32* pagetable, uint32 va);

void copyout(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size);

void copyin(uint32* pagetable, uint32* k_pa, uint32  u_va, uint32 size);

void unmap_vm(uint32* pagetable);

void map_va_to_pa(uint32* pagetable, uint32 va, uint32* pa, int perms );

void copy_proc_mem(uint32* parent_pagetable, uint32* child_pagetable);

void map_trampoline_and_trapframe(uint32* pagetable, uint32* trapframe);

uint32 get_internal_page_no(uint32 va);

uint32 get_leaf_page_no(uint32 va);

uint32 extract_pa_from_pte(uint32 pte);

uint32 extract_perms_from_pte(uint32 pte) ;

int set_perms(char* code);
