
#define MAX_PROC 10
#define KMEM_START 2147483648
#define KMEM_SIZE  1048576 // 1 MB
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct trapframe {
  unsigned int kernel_satp;   // kernel page table
  unsigned int kernel_sp;     // top of process's kernel stack
  unsigned int kernel_trap;   // usertrap()
  unsigned int epc;           // saved user program counter
  unsigned int kernel_hartid; // saved kernel tp
  unsigned int ra;
  unsigned int sp;
  unsigned int gp;
  unsigned int tp;
  unsigned int t0;
  unsigned int t1;
  unsigned int t2;
  unsigned int s0;
  unsigned int s1;
  unsigned int a0;
  unsigned int a1;
  unsigned int a2;
  unsigned int a3;
  unsigned int a4;
  unsigned int a5;
  unsigned int a6;
  unsigned int a7;
  unsigned int s2;
  unsigned int s3;
  unsigned int s4;
  unsigned int s5;
  unsigned int s6;
  unsigned int s7;
  unsigned int s8;
  unsigned int s9;
  unsigned int s10;
  unsigned int s11;
  unsigned int t3;
  unsigned int t4;
  unsigned int t5;
  unsigned int t6;
};

struct context {
  unsigned int ra;
  unsigned int sp;

  // callee-saved
  unsigned int  s0;
  unsigned int  s1;
  unsigned int  s2;
  unsigned int  s3;
  unsigned int  s4;
  unsigned int  s5;
  unsigned int  s6;
  unsigned int  s7;
  unsigned int  s8;
  unsigned int  s9;
  unsigned int  s10;
  unsigned int  s11;
};

typedef struct proc
{
    enum procstate state;
    void* chan;
    int killed;
    int xstatus;
    int pid;
    uint32*  kstatck;
    unsigned int size;
    struct proc* parent;
    uint32*  pagetable;
    struct trapframe* ptr_to_trapframe;
    struct context context;
    char name[16];
} PROC;

void procinit(void);

void userinit(void);