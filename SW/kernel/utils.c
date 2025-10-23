#include "utils.h"

//Write a word at addr
void writei(int val, uint32* addr)
{
    *addr = val;
}

//write the same value from the given start addr to size bytes
//src should be 4 byte aligned
int memstr(uint32* src, int val, uint32 size)
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
void memcpy(uint32* dest, uint32* src, uint32 size)
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