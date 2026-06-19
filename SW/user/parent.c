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
char buff[10] = {'a','a','a','a'};
int break_flag = 0;
int main()
{
    sys_read(buff,5);

    sys_read(buff,4);

    while(1)
    {
        sys_read(buff,4);
    }
}