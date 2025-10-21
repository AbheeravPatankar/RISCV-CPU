#include "paging.h"
#include "proc.h"


int main(void)
{
    initpaging();   // link all the free pages in the user space to the page_list 
    procinit();

    // userinit() - create the first user process and party starts !!!
    return 0;
}
