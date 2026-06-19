// this file defines all the sys call codes that the OS will provide 
#pragma once
#include "defines.h"

#ifndef __ASSEMBLER__
int sys_fork(void);
int sys_exit(int status);
int sys_exec(char* name);
int sys_wait();
void* sys_sbrk(uint32 size);
void* sys_read(void* buffer, uint32 size);
#endif