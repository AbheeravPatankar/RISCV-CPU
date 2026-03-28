/*
    program to test fork exec
*/
#include "ulib.h"

char some_data[10] = {1,2,3,4,5,6,7,8,9};

int main()
{
    int* mem = (int*)malloc(100);
    char* mem2 = (char*)malloc(200);

    /* write some bytes into mem */
    mem[0] = 0xDEADBEEF;
    mem[1] = 0xCAFEBABE;
    mem[2] = 0x12345678;

    /* write some bytes into mem2 */
    mem2[0] = 'A';
    mem2[1] = 'B';
    mem2[2] = 'C';
    mem2[3] = 0x55;
    mem2[4] = 0xAA;

    /* loop infinitely */
    while(1);
}

