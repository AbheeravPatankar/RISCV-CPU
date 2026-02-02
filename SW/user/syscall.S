#include "syscall.h"

.global sys_fork
sys_fork:
    li a7, SYS_fork
    ecall 
    ret

.global sys_exit
sys_exit:
    li a7, SYS_exit
    ecall
    ret