/*
    program to test fork exec
*/
#include "syscall.h"


int jump_even()
{
    return 0;
}


int jump_odd()
{
    return 1;
}


int print_odd()
{
    while(1)
    {
        for(int i = 0 ; i < 100; i++)
        {
            if(i % 2 == 0)
            {
                print_even();
            }
        }
    }
}


int print_even()
{
    while(1)
    {
        for(int i = 0 ; i < 100; i++)
        {
            if(i % 2 == 0)
            {
                print_even();
            }
        }
    }
}


int main()
{
    // call fork and exit 

    int pid = sys_fork();

    if(pid == 0)
    {
        print_even();
    }
    else
    {
        print_odd();
    }


}




