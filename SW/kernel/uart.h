#pragma once

#define BUFFER_SIZE 5120


// dummy process until uart implementatio is done 
SEGMENT_HEADER* get_proc_elf_header(char* name)
{
    return (SEGMENT_HEADER*)0x80005000 ;
}