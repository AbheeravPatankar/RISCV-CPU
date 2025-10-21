#include "utils.h"

//Write a word at addr
void writei(int val, int* addr)
{
    *addr = val;
}

//write the same value from the given start addr to size bytes
//src should be 4 byte aligned
int memstr(int* src, int val, int size)
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
    int* addr = BAR + offset * 4;
    *addr = val;
}