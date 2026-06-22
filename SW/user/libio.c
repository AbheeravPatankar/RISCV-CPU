#include "libio.h"
#include "defines.h"
#include "heap.h"
uint32 args[16]; 
// copy data from one physical address to other 
void u_memcpy(char* dest, char* src, uint32 size)
{
    for(int i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
} 


void read_arg(char* arg_vec, int idx, void* buff, int buff_idx)
{
    if(idx > 7)
    {
        // read the argument from the stack
        u_memcpy(buff + buff_idx, arg_vec, 4);
        arg_vec += 4;
    }
    else
    {
        // read the argument from the already-spilled register block
        // idx 0 corresponds to a1 (the first variadic arg), since a0 = str
        uint32 val = args[idx];
        u_memcpy(buff + buff_idx, &val, 4);
        
    }
}

void store_int_args(char* arg_vec, char* arg_str, int* int_buff, uint32* uint_buff,  uint32* ptr_buff)
{
    char c;
    int i = 0;
    uint32 arg_count = 0;
    uint32 int_buff_idx = 0;
    uint32 uint_buff_idx = 0;
    uint32 ptr_buff_idx = 0;


    while(c != 0)
    {
        c = arg_str[i];
        if(c == '%')
        {
            // its an access specifier
            switch (arg_str[i + 1])
            {
            case 'd':
                // its an integer
                read_arg(arg_vec, arg_count,(void*)int_buff, int_buff_idx);
                break;
            case 'u':
                // its a pointer
                read_arg(arg_vec, arg_count,(void*)uint_buff, uint_buff_idx);
                break;
            case 'p':
                // its an unsigned int
                read_arg(arg_vec, arg_count,(void*)ptr_buff, ptr_buff_idx);
                break;
            case 'c':
                if(arg_count > 7)
                    arg_vec++;
                break;
            default:
                break;
            }
            i++;
            arg_count++;
        }
        i++;
    }
}

void get_int_arg_count(char* arg_str, int* counts)
{
    char c;
    int i = 0;
    while(c != 0)
    {
        c = arg_str[i];
        if(c == '%')
        {
            // its an access specifier
            switch (arg_str[i + 1])
            {
            case 'd':
                // its an integer
                counts[1]++;
                break;
            case 'c':
                // its a character
                counts[0]++;
                break;
            case 'p':
                // its a pointer
                counts[3]++;
                break;
            case 'u':
                // its an unsigned int
                counts[2]++;
                break;
            default:
                break;
            }
            i++;
        }
        i++;
    }
}

int rv_printf(char* str, ...)
{
    // enough for register args + some stack-spilled ones; size to your needs
    char *arg_vec;

    // step 1: spill a1-a7 into args[0..6]
    asm volatile (
        "sw   a1, 0(%0)   \n"
        "sw   a2, 4(%0)   \n"
        "sw   a3, 8(%0)   \n"
        "sw   a4, 12(%0)  \n"
        "sw   a5, 16(%0)  \n"
        "sw   a6, 20(%0)  \n"
        "sw   a7, 24(%0)  \n"
        :
        : "r"(args)
        : "memory"
    );

    // step 2: grab s0, pointing at the caller's stack-spilled args (8th onward)
    asm volatile ("mv %0, s0" : "=r"(arg_vec));

    char* ptr_to_str;
    uint32 char_count = 0;
    uint32 arg_count = 0;
    asm volatile ("mv %0, a0" : "=r"(ptr_to_str));

    // counts[0] -> char count 
    // counts[1] -> int count
    // counts[2] -> uint count
    // counts[3] -> ptr count
    int counts[4] = {0,0,0,0};

    // create buffers for all the integer arguemnts

    int* int_buff = malloc(sizeof(int) * counts[1]);
    uint32* uint_buff = malloc(sizeof(uint32) * counts[2]);
    uint32* ptr_buff = malloc(sizeof(uint32) * counts[3]);

    get_int_arg_count(ptr_to_str, counts);

    store_int_args(arg_vec, ptr_to_str, int_buff, int_buff, ptr_buff);
}