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

 .global sys_exec
sys_exec:
    li a7, SYS_exec
    ecall
    ret

.global sys_wait
sys_wait:
    li a7, SYS_wait
    ecall
    ret