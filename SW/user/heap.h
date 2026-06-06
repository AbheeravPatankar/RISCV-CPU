#pragma once
#include "defines.h"


// This header is placed at the start of the hole which is greater than 32 bytes 
typedef struct hole_32_bytes
{
    uint32* base_addr;
    uint32  size;
    struct hole_32_bytes* right;
    struct hole_32_bytes* left;
    
}HOLE_32B;


// This header is placed at the start of the allocated memory by calling malloc()
typedef struct mem_allocation_header
{
    uint32 magic;
    uint32 size;
}MEM_ALLOCATION_HEADER;

// L32B = less that 32 bytes
// This header is placed at the start of the hole which is less than 32 bytes 
typedef struct hole_less_that_32_bytes
{
    struct hole_less_that_32_bytes* next;
}HOLE_L32B;


typedef struct heap_header
{
    // holes having size >= 4B
    HOLE_32B* head_32b;

    /*
        array_of_head[0] = head ptr to ll of holes size = 4B
        array_of_head[1] = head ptr to ll of holes size = 8B
        array_of_head[2] = head ptr to ll of holes size = 12B
        array_of_head[3] = head ptr to ll of holes size = 16B
        array_of_head[4] = head ptr to ll of holes size = 20B
        array_of_head[5] = head ptr to ll of holes size = 24B
        array_of_head[6] = head ptr to ll of holes size = 28B
    */
    HOLE_L32B* array_of_head_l32b[7];
}HEAP_HEADER;


void* malloc(uint32 size);

int mfree(void* base_addr);