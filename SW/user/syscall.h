// this file defines all the sys call codes that the OS will provide 

#define SYS_fork    1
#define SYS_exit    2
#define SYS_exec    3
#define SYS_wait    4

#ifndef __ASSEMBLER__
int sys_fork(void);
int sys_exit(int status);
int sys_exec(char* name);
int sys_wait();
#endif