#include "proc.h"
#include "trap.h"
#include "uart.h"
#include "kernel_vm.h"

extern char* userret;
extern char* uservec;

extern uint32 kernel_satp;

PROC processes[MAX_PROC];

PROC* initproc;

CPU cpu = {0};  // only one cpu object as the system is unicore 

PROC* myproc()
{
    return cpu.current_proc;
}

// round up the proc_size to page aligned 
uint32 size_roundup(uint32 size)
{
    if(size % PAGE_SIZE == 0)
        return size;
    else
    {
        size = PAGE_SIZE * (size / PAGE_SIZE + 1);
        return size;
    }
}

uint32 size_rounddown(uint32 size)
{
    if(size % PAGE_SIZE == 0)
        return size;
    return (size / PAGE_SIZE) * PAGE_SIZE;
}

int alloc_pid(void)
{
    static int pid_counter = -1;   
    pid_counter++;
    return pid_counter;
}

//initialize the process table 
void procinit()
{
    unsigned int kmem_end = KMEM_START + KMEM_SIZE;
    for(int i = 0; i < 10; i++)
    {
        //init Kstack for 10 processes (max proc = 10)
        processes[i].state = UNUSED;
        processes[i].kstack = (uint32*)(kmem_end - (i + 1) * PAGE_SIZE);
    }
}

//find unused proc struct
PROC* alloc_proc()
{
    for(int i = 0; i < MAX_PROC; i++)
    {
        if(processes[i].state == UNUSED)
        {
            processes[i].pid = alloc_pid();

            // allocate that structure member and return
            PAGE* trapframe = alloc_page();
            processes[i].state = USED;

            // allocate the trapframe page 
            processes[i].ptr_to_trapframe = trapframe;
            return &processes[i];
        }
    }
    return NULL;
}

// fork the process - make the copy of the process 

int kfork()
{
    PROC* parent = myproc();
    PROC* child = alloc_proc();
    if(child == NULL)
        return ;

    PAGE* pagetable = create_page_table();
    child->pagetable = (uint32*)(pagetable);

    // copy the entire parent process memory in child process memory 
    copy_proc_mem(parent->pagetable, child->pagetable);
    child->size = parent->size;

    map_trampoline_and_trapframe(child->pagetable, child->ptr_to_trapframe);
    //copy the trapframes 
    k_memcpy(child->ptr_to_trapframe, parent->ptr_to_trapframe, sizeof(struct trapframe));

    // return 0 for child process 
    child->ptr_to_trapframe->a0 = 0;

    // copy the name of the process
    k_strcpy(child->name, parent->name);

    child->parent = parent;

    child->context.ra = fork_ret;
    child->context.sp = child->ptr_to_trapframe->kernel_sp;
    child->state = RUNNABLE;

    // return the child pid for parent process
    return child->pid;
}


// function to parse the segment header
void parse_segment_header(SEGMENT_HEADER* seg_header, uint32* base_addr, PROC* p)
{ 
    // more pages are required to accomodate the process
    uint32 start_vaddr = size_rounddown(seg_header->vaddr);
    uint32 read_size = seg_header->filesz + seg_header->vaddr - start_vaddr;
    map_vm(p->pagetable,start_vaddr , read_size , set_perms("RWXUV"));
    
    // copy memory from the buffer to allocated pages
    // !NOTE copyout is not going to work for processes having segments larger that BUFFER_SIZE -- replace this with some uart read function  
    copyout(p->pagetable, (char*)base_addr + seg_header->offset, seg_header->vaddr, seg_header->filesz);
}

// initialize the a process 
// this is actually exec call implementation - ( make the necessary changes )
int kexec(char* name)
{
    PROC* p = myproc();
    PAGE* pagetable = alloc_page();
    PAGE* old_pagetable = NULL;
    if(p->pagetable != NULL)
        old_pagetable = p->pagetable;
    ELF_HEADER* elf_header;

    // write code to signal uart to get the elf header
    if(strcmp("init", name))
        elf_header = (ELF_HEADER*) 0x80005000;
    else
        elf_header = get_proc_elf_header(name);

    // write the proc name
    for(int i = 0 ; i < 16; i++)
    {
        if(name[i] == '\0')
            break;
        p->name[i] = name[i];
    }
    
    //assign the newly created pagetable to the process 
    p->pagetable = pagetable ;
    // parse the header and allocate pages
    if(elf_header->magic != 1234)
    {
        return -1 ;
    }
    // assign the epc to entry from the header as userret executes  mret at the end , 
    p->ptr_to_trapframe->epc = elf_header->entry ;


    uint32 segment_offset = elf_header->segment_offset;
    const uint32 seg_header_size = 20;
    for(int i = 0; i < elf_header->segment_count; i++)
    {
        // parse each segment header ( code and data segments )
        SEGMENT_HEADER* seg_header = (SEGMENT_HEADER*)((char*)elf_header + segment_offset + i * seg_header_size);
        if(seg_header == NULL)
            return -1;
        // if not null then parse the segment header 
        parse_segment_header(seg_header, elf_header, p);
        p->size += seg_header->filesz ;
    }

    // allocate a stack page of user stack and assign to sp in trapframe 
    PAGE* ustack = alloc_page();
    map_va_to_pa((uint32*)pagetable, size_roundup(p->size) + 4096 * 2 , (uint32)ustack, set_perms("RWXUV"));
    p->ptr_to_trapframe->sp = size_roundup(p->size) + 4096 * 2 + PAGE_SIZE;


    // asign the size of proc which is code + data + stack
    p->size = size_roundup(p->size) + PAGE_SIZE;
    
    // map the trampoline and trapframe section
    map_trampoline_and_trapframe((uint32*) p->pagetable, (uint32*)p->ptr_to_trapframe);
    p->killed = 0;
    
    // map the address of usertrap in trapframe->kernel trap 
    p->ptr_to_trapframe->kernel_trap = (uint32)usertrap;

    // map the kernel pagetable in trapframe
    p->ptr_to_trapframe->kernel_satp = kernel_satp;

    // free all pages in the old pagetable
    if(old_pagetable != NULL)
        unmap_vm(old_pagetable);

    return 0;
}

// function to initialize the first user process ( process which will invoke the shell )
void userinit()
{
    // allocate the proc structure and trapframe to the first process 
    PROC* p = alloc_proc();
    initproc = p;
    p->state = RUNNABLE;
    p->context.sp = p->kstack + PAGE_SIZE;
    p->context.ra = fork_ret;

    // process is ready to be swtchled now 
    return ;
}


void fork_ret()
{
    prepare_return();
    PROC* p = myproc();

    uint32 root_pa  = (uint32)(p->pagetable);  // ROOT page table
    uint32 root_ppn = root_pa >> 12;
    uint32 satp = (1U << 31) | root_ppn;
    uint32 test = sv32_va_to_pa(satp, 0xbc);
    ((void (*)(uint32))userret)(satp);
}


// function through the process will give away its timeshare
// called after a timer interrupt 
void  kyeild()
{
    PROC* p = myproc();
    p->state = RUNNABLE;
    
    // call switch to switch context from this thread to the swtchuler thread

    if(p->state == RUNNING)
        return ;

    swtch(&(p->context), &(cpu.context));
}

int killed(PROC* p)
{
    return p->killed;
}


void reparent(PROC* p)
{
    int ppid = p->pid;

    for(uint32 i = 0 ; i < MAX_PROC ; i++)
    {
        if(processes[i].parent == p)
        {
            // reparent to init
            processes[i].parent = initproc;

            // TODO wakeup init proc - to remove all the zomblie children
            kwakeup(initproc);
        }
    }
}

void kexit(int status)
{
    // loop unitl we define the function correctly 
    PROC* p = myproc();

    if(p == initproc)
        return ;

    // reparent all the child processes 
    reparent(p);

    p->state = ZOMBIE;
    p->xstatus = status ;

    kwakeup(p->parent);
}


void kwakeup(void* chan)
{
    for(int i = 0 ; i < MAX_PROC; i++)
    {
        if((processes + i) != myproc())
        {
            if(processes[i].chan == chan)
            {
                processes[i].state = RUNNABLE;
            }
        }
    }
}

void ksleep(void* chan)
{
    PROC* p = myproc();

    p->chan = chan;
    p->state = SLEEPING;

    swtch(&(p->context), &(cpu.context));

    p->chan = NULL;

}

// need to return 
int kwait(uint32* addr)
{
    PROC* parent = myproc();
    PROC* child;

    int havekids,pid;

    while(1)
    {
        havekids = 0;
        for(int i = 0 ; i < MAX_PROC; i++)
        {
            // check if there is a child to the current process
            if(processes[i].parent == parent)
            {
                havekids = 1;

                // check if the child has exited
                if(processes[i].state == ZOMBIE)
                {
                    // copy the exit state 
                    if(addr != NULL)
                    {
                        copyout((uint32*)parent->pagetable,&(child->xstatus), (uint32)addr, sizeof(child->xstatus));
                    }
                    else
                        return -1;

                    // copy the pid 
                    pid = child->pid;

                    // free the child process memory 
                    freeproc(child);

                    return pid;
                    
                }
            }     
        }

        // return if there were no children     
        if(havekids == 0)
        {
            return -1;
        }

        // sleep until child calls exit 
        ksleep(parent);
        
    }
}



// void kkill()

// void freeproc() free all pages occupied by the proc , free its trapframe , free the proc structure 


void freeproc()
{
    while(1);
}
