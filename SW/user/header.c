/* This is a generated file */

typedef unsigned int uint32;

typedef struct elf_header
{
    uint32 magic;
    uint32 entry;
    int segment_count;
    uint32 segment_offset;
} ELF_HEADER;

typedef struct segment_header
{
    int flags;
    uint32 offset;
    uint32 vaddr;
    uint32 filesz;
    uint32 memsz;
} SEGMENT_HEADER;

__asm__(".section .header,\"aw\",@progbits");
__asm__(".align 16");

ELF_HEADER elf_header __attribute__((section(".header"), aligned(16))) = {1234, 0x0000205c, 2, 16};

SEGMENT_HEADER segment_header[2] __attribute__((section(".header"), aligned(16))) = {
    {
        4, 0x00000040, 0x00002000, 2936, 4096
    },
    {
        4, 0x00000bb8, 0x00002b78, 24, 4096
    }
};

char padding[8] __attribute__((section(".header"))) = {};
