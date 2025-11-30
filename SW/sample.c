/*
=========================================================== GPL( God Public Liscense ) =============================================================
Author : The real GOD !
To 
People on earth , 
This is a program that constructs an argument buffer of any data type arguments and also parses the buffer after it is passed to some function.
This will allow you to define functions which can accept variable number of arguments of any data type in c !! 

No need to say thanks ! 
===================================================================================================================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void construct_arg_buffer(char** buff, int dtype, char* src_addr)
{
    static int buffer_index = 0;
    int size = 0;

    switch (dtype)
    {
        case 1:         // int 
            size = 4;
            break;       
        case 2:         // float
            size = 4;
            break;  
        case 3:         // char 
            size = 1;
            break;
        case 4:         // double 
            size = 8;
            break;
        default:
            return ;
    }

    if(*buff == NULL)
    {
        *buff = (char*)malloc(size);
        buffer_index = 0;
    }
    else
    {
        *buff = (char*)realloc(*buff, buffer_index + size);
    }

    memcpy(*buff + buffer_index, src_addr, size);
    buffer_index += size;
    return ;
}

//===================================================================== User space =========================================================================

void print_args(char* arg_s , char* arg_v)
{
    int index = 0;
    while(*arg_s != 0)
    {    
        char dtype = *arg_s;
        switch(dtype)
        {
            case 'i':       // int
                printf("%d \n", *(int*)(arg_v + index));
                index += 4;
                break;
            case 'f':      // float 
                printf("%f \n", *(float*)(arg_v + index));
                index += 4;
                break;
            case 'c':       // char 
                printf("%c \n", *(char*)(arg_v + index));
                index += 1;
                break;
            case 'd':       // double 
                printf("%lf \n", *(double*)(arg_v + index));
                index += 8;
                break;
            default : 
                break;
        }
        arg_s++;
    }
}


int main()
{
    
    int arg1 = 10;
    int arg2 = 20;
    char arg3 = 'a';
    float arg4 = 3.14;
    char* arg_buff = NULL;
    int argc = 1;
    char* arg_s = (char*)malloc(1);
    
    
    // instantiate the buffer with some arguments 
    construct_arg_buffer(&arg_buff, 1, (char*)&arg1);
    arg_s[argc - 1] = 'i';
    arg_s = (char*)realloc(arg_s, ++argc);
    construct_arg_buffer(&arg_buff, 1, (char*)&arg2);
    arg_s[argc - 1] = 'i';
    arg_s = (char*)realloc(arg_s, ++argc);
    construct_arg_buffer(&arg_buff, 3, (char*)&arg3);
    arg_s[argc - 1] = 'c';
    arg_s = (char*)realloc(arg_s, ++argc);
    construct_arg_buffer(&arg_buff, 2, (char*)&arg4);
    arg_s[argc - 1] = 'f';
    arg_s = (char*)realloc(arg_s, ++argc);

    // print the buffer 
    print_args(arg_s, arg_buff);

    return 0;
}
