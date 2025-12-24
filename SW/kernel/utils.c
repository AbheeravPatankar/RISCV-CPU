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