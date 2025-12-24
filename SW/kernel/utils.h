
#pragma once

#define BAR (uint32*)3072
#define NULL ((void *)0)

typedef unsigned int uint32 ;

extern char trampoline[]; 

void writei(int val, uint32* addr);

int memstr(char* src, int val, uint32 size);

void write_scratch(int val, int offset);

void k_memcpy(char* dest, char* src, uint32 size);

int max(int a, int b);

int binaryToDecimal(int* arr, int size);

void k_strcpy(char* dest, char* src);

int strcmp(char* str1, char*str2);