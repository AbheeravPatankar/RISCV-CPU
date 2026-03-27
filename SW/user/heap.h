#pragma once
#include "defines.h"
typedef struct hole_4_bytes
{
    uint32* base_addr;
    uint32  size;
    struct hole_4_bytes* right;
    struct hole_4_bytes* left;
    
}HOLE_4B;

// L$B = less that 32 bytes
typedef struct hole_less_that_32_bytes
{
    struct hole_less_that_32_bytes* next;
}HOLE_L32B;


typedef struct heap_header
{
    // holes having size >= 4B
    HOLE_4B* head_4b;

    /*
        array_of_head[0] = head ptr to ll of holes size = 4B
        array_of_head[1] = head ptr to ll of holes size = 8B
        array_of_head[2] = head ptr to ll of holes size = 12B
        array_of_head[3] = head ptr to ll of holes size = 16B
        array_of_head[4] = head ptr to ll of holes size = 20B
        array_of_head[5] = head ptr to ll of holes size = 24B
        array_of_head[6] = head ptr to ll of holes size = 28B
        next 8 byte padding 
    */
    HOLE_L32B* array_of_head_l32b[7];
}HEAP_HEADER;


uint32* malloc(uint32 size);

void free(void* base_addr);