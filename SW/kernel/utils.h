

#define BAR (uint32*)3072
#define NULL ((void *)0)
typedef unsigned int uint32 ;

void writei(int val, uint32* addr);

int memstr(uint32* src, int val, uint32 size);

void write_scratch(int val, int offset);

void memcpy(uint32* dest, uint32* src, uint32 size);

int max(int a, int b);