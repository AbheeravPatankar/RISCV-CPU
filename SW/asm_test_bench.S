.section .text
    .globl _start

_start:
    addi ra, x0, 10     
    li t0, 2000         
    sb ra, 0(t0)        
    addi ra, x0, 11
    li t0, 2001        
    sb ra, 0(t0)
    addi ra, x0, 12     
    li t0, 2002         
    sb ra, 0(t0)
    addi ra, x0, 13     
    li t0, 2001         
    sb ra, 0(t0)
    li t0, 2001
    lb sp,0(t0)