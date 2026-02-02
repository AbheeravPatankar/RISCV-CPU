// this file defines all the sys call codes that the OS will provide 

#define SYS_fork    1
#define SYS_exit    2

#ifndef __ASSEMBLER__
int sys_fork(void);
int sys_exit(int status);
#endif