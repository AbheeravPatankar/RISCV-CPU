/*
    program to test fork exec
*/
#include "ulib.h"

void fill(char* start, uint32 size)
{
    for(int i = 0 ; i < size ; i++)
    {
        start[i] = 10;
    }
}


char some_data[10] = {1,2,3,4,5,6,7,8,9};
int break_flag = 0;
int main()
{
    int* mem = (int*)malloc(100);
    fill((char*)mem,100);

    char* mem2 = (char*)malloc(200);
    fill((char*)mem2,200);

    mfree(mem);

    mem = (int*)malloc(250);
    fill((char*)mem,250);

    /* loop infinitely */
    while(1);
}