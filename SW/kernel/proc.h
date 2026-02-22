#pragma once

#include "utils.h"
#include "paging.h"

#define MAX_PROC 10

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct trapframe {
  uint32 kernel_sp;     // top of process's kernel stack
  uint32 kernel_trap;   // usertrap()
  uint32 kernel_satp;
  uint32 epc;           // saved user program counter
  uint32 kernel_hartid; // saved kernel tp
  uint32 ra;
  uint32 sp;
  uint32 gp;
  uint32 tp;
  uint32 t0;
  uint32 t1;
  uint32 t2;
  uint32 s0;
  uint32 s1;
  uint32 a0;
  uint32 a1;
  uint32 a2;
  uint32 a3;
  uint32 a4;
  uint32 a5;
  uint32 a6;
  uint32 a7;
  uint32 s2;
  uint32 s3;
  uint32 s4;
  uint32 s5;
  uint32 s6;
  uint32 s7;
  uint32 s8;
  uint32 s9;
  uint32 s10;
  uint32 s11;
  uint32 t3;
  uint32 t4;
  uint32 t5;
  uint32 t6;
};

struct context {
  uint32 ra;
  uint32 sp;

  // callee-saved
  uint32  s0;
  uint32  s1;
  uint32  s2;
  uint32  s3;
  uint32  s4;
  uint32  s5;
  uint32  s6;
  uint32  s7;
  uint32  s8;
  uint32  s9;
  uint32  s10;
  uint32  s11;
};

typedef struct proc
{
    enum procstate state;
    void* chan;
    int killed;
    int xstatus;
    int pid;
    uint32*  kstack;
    unsigned int size;
    struct proc* parent;
    uint32*  pagetable;
    struct trapframe* ptr_to_trapframe;
    struct context context;
    char name[16];
} PROC;

typedef struct elf_header
{
  uint32 magic;
  uint32 entry;          // address of main function in the program
  int segment_count;     // Number of segments this executable has 
  uint32 segment_offset; // offset from where the segment headers can be read 

}ELF_HEADER;

typedef struct segment_header
{
  int flags;
  uint32 offset; // address in the user bin where the section starts 
  uint32 vaddr;  // address in the user bin where this segment is to be loaded 
  uint32 filesz; // size of the segment
  uint32 memsz;  // size in mem that the segment will occupy
}SEGMENT_HEADER;

typedef struct cpu
{
  PROC* current_proc;
  struct context context;
  int int_ena;
}CPU;


void procinit(void);

void userinit(void);

void scheduler();

void swtch(struct context* old, struct context* new);

int kexec(char* name);

void fork_ret();

void kyeild();

void sched();

int kfork();

void kexit(int status);

void kwakeup(void* chan);