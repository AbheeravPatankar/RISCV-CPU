#include "libio.h"
#include "defines.h"
#include "heap.h"
#include "syscall.h"



uint32 get_str_len(char* ptr_to_str)
{
    char c = ptr_to_str[0];
    uint32 len = 0;
    while(c != 0)
    {
        c = ptr_to_str[len];
        len++;
    }

    return len - 1;
}


// copy data from one physical address to other 
void u_memcpy(char* dest, char* src, uint32 size)
{
    for(int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
} 


char* convert_int_to_chars(int value, char *buff)
{
    char tmp[12];          // enough for -2147483648
    int i = 0;
    unsigned int num;

    if (value == 0)
    {
        *buff++ = '0';
        return buff;
    }

    if (value < 0)
    {
        *buff++ = '-';

        // avoid overflow for INT_MIN
        num = (unsigned int)(-(value + 1)) + 1;
    }
    else
    {
        num = (unsigned int)value;
    }

    while (num > 0)
    {
        tmp[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0)
    {
        *buff++ = tmp[--i];
    }

    return buff;
}


// this function will read the argument and write into the buffer and return the address 
// to which the next character will be written in the buffer
char* read_arg(char* arg_vec, uint32 arg_count, char* buff, uint32 size)
{
    int tmp_int = 0;
    char tmp_char;

    tmp_int = *(int *)(arg_vec + arg_count * 4);
    
    if(size == 1)
    {
        tmp_char = (char)tmp_int;
        *buff = tmp_char;
        buff++;
    }
    else 
    {
        // its an integer so will have to convert that integer to characters and write those characters
        buff = convert_int_to_chars(tmp_int,buff); 

    }
    // copy the argument into the buffer
    
    arg_vec += 4;

    return buff;
}

uint32 format_string(char* ptr_to_str, uint32 str_len, char* str_buff, char* arg_vec)
{
    uint32 arg_count = 0;
    char* tmp_buff = str_buff;
    for(int i = 0; i < str_len; i++)
    {
        if(ptr_to_str[i] == '%')
        {
            // access specifier
            // its an access specifier
            char c = ptr_to_str[i + 1];
            if(c == 'p' || c == 'd' || c == 'u')
            {
                // read a 4 byte arg
                tmp_buff = read_arg(arg_vec, arg_count, tmp_buff , 4);
                
            }
            else if ( c == 'c') 
            {
                // read a char
                tmp_buff = read_arg(arg_vec, arg_count, tmp_buff , 1);
            }
            else 
            {
                // invalid access specifier , address this case differntly
                return NULL;
            }
            arg_count++;
            i++;
        }
        else 
        {
            // normal character
            *tmp_buff = ptr_to_str[i];
            tmp_buff++;
        }
    }

    return (uint32)(tmp_buff - str_buff);
}


void scan_string(char* ptr_to_str, uint32 str_len)
{
    uint32 arg_count = 0;
    for(int i = 0; i < str_len; i++)
    {
        if(ptr_to_str[i] == '%')
        {
            // access specifier
            // its an access specifier
            char c = ptr_to_str[i + 1];
            if(c == 'p' || c == 'd' || c == 'u')
            {
                // read an integer from the console
                uint32 tmp = 0;
                sys_read(&tmp, 4);                
            }
            else if ( c == 'c') 
            {
                
            }
            else 
            {
                
            }
            arg_count++;
            i++;
        }
        // do nothing on a normal character 
    }
}

/*
============================================================================================================
                These are the functions exposed to the user 
*/


int rv_printf(char* str, ...)
{
    // enough for register args + some stack-spilled ones; size to your needs
    char *arg_vec;

    // step 2: grab s0, pointing at the caller's stack-spilled args (8th onward)
    asm volatile ("mv %0, s0" : "=r"(arg_vec));
    arg_vec = arg_vec + 4;


    char* ptr_to_str;

    asm volatile ("mv %0, a0" : "=r"(ptr_to_str));

    uint32 string_len = 0;
    string_len = get_str_len(ptr_to_str);

    // A random buffer allocation for now  ... we will calculate and allocate the buffer in the future
    char* str_buff = (char*)rv_malloc(string_len + 30);
    string_len = format_string(ptr_to_str, string_len, str_buff, arg_vec);
    sys_write(str_buff,string_len);

    rv_free((void*)str_buff);

    return string_len;
}


int rv_scanf(char* str, ...)
{
    // enough for register args + some stack-spilled ones; size to your needs
    char *arg_vec;

    // step 2: grab s0, pointing at the caller's stack-spilled args (8th onward)
    asm volatile ("mv %0, s0" : "=r"(arg_vec));
    arg_vec = arg_vec + 4;

    char* ptr_to_str;

    asm volatile ("mv %0, a0" : "=r"(ptr_to_str));

    uint32 string_len = 0;
    string_len = get_str_len(ptr_to_str);

}