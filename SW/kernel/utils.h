
#pragma once

#define BAR (uint32*)3072
#define NULL ((void *)0)

typedef unsigned int uint32 ; 

void writei(int val, uint32* addr);

int memstr(char* src, int val, uint32 size);

void write_scratch(int val, int offset);

void k_memcpy(char* dest, char* src, uint32 size);

int max(int a, int b);

int min(int a, int b);

int binaryToDecimal(int* arr, int size);

void k_strcpy(char* dest, char* src);

int strcmp(char* str1, char*str2);

uint32 roundup(uint32 val, uint32 multiple);

typedef uint32 pte_t;
#define PTE_V   (1 << 0)
#define PTE_R   (1 << 1)
#define PTE_W   (1 << 2)
#define PTE_X   (1 << 3)

uint32 sv32_va_to_pa(uint32 satp, uint32 va);