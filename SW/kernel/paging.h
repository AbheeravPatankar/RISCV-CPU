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

