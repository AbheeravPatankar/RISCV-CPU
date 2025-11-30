#include "proc.h"
#include "trap.h"

// function to allocate new pid for the process

PROC processes[MAX_PROC];

CPU cpu = {0};  // only one cpu object as the system is unicore 

PROC* myproc()
{
    return cpu.current_proc;
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
        processes[i].kstatck = (uint32*)(kmem_end - (i + 1) * PAGE_SIZE);
    }
}

//find unused proc struct
PROC* alloc_proc()
{
    for(int i = 0; i < MAX_PROC; i++)
    {
        if(processes[i].state == UNUSED)
        {
            // allocate that structure member and return 
            processes[i].state = USED;
            return &processes[i];
        }
    }
    return NULL;
}

// function to parse the segment header
void parse_segment_header(SEGMENT_HEADER* seg_header, uint32* base_addr, PROC* p)
{
    // allocate pages to copy the segment
    if(seg_header->memsz % PAGE_SIZE != 0)
        return ;
    int n_pages = seg_header->memsz / PAGE_SIZE;
    // create the entries for page table  
    map_vm(p->pagetable, seg_header->vaddr, seg_header->memsz, seg_header->flags);
    // copy memory from the buffer to allocated pages
    // !NOTE copyout is not going to work for processes having segments larger that BUFFER_SIZE -- replace this with some uart read function  
    copyout(p->pagetable, (char*)base_addr + seg_header->offset, seg_header->vaddr, seg_header->filesz);
}

// initialize the a process 
PROC* usr_proc_create(ELF_HEADER* elf_header, char* name)
{
    int pid = alloc_pid();
    PROC* p = alloc_proc();
    if(p == NULL)
        return ;
    p->pid = pid;
    //write the proc name
    for(int i = 0 ; i < 16; i++)
    {
        if(name[i] == '\0')
            break;
        p->name[i] = name[i];
    }

    PAGE* pagetable = create_page_table();
    p->pagetable = (uint32*)(pagetable);
    
    // parse the header and allocate pages
    if(elf_header->magic != 1234)
    {
        return ;
    }
    // assign the ra to entry from the header 
    p->context.ra = elf_header->entry;
    uint32 segment_offset = elf_header->segment_offset;
    const uint32 seg_header_size = 20;
    uint32 pages = 0;
    for(int i = 0; i < elf_header->segment_count; i++)
    {
        // parse each segment header ( code and data segments )
        SEGMENT_HEADER* seg_header = (SEGMENT_HEADER*)((char*)elf_header + segment_offset + i * seg_header_size);
        if(seg_header == NULL)
            return ;
        // if not null then parse the segment header 
        parse_segment_header(seg_header, elf_header, p);
        pages += seg_header->memsz / PAGE_SIZE ;
    }
    pages++;
    PAGE* ustack = alloc_page();
    map_va_to_pa(pagetable,pages * PAGE_SIZE, (uint32*)ustack, "RWU");
    // asign the size of proc which is code + data + stack
    p->size = pages * PAGE_SIZE;
    // map the trapframe and trampoline section
    map_va_to_pa((uint32*)pagetable, pages * PAGE_SIZE , (uint32*)trampoline , "XU");
    PAGE* trapframe = alloc_page();
    map_va_to_pa((uint32*)pagetable, (pages + 1) * PAGE_SIZE, (uint32*)trapframe, "RW");

    
    p->ptr_to_trapframe = trapframe;
    p->killed = 0;
   
    return p;
}

// function to initialize the first user process ( process which will invoke the shell )
void userinit()
{
    char name[16] = "init";
    PROC* p = usr_proc_create((SEGMENT_HEADER*)0x800FC400, name);
    p->parent = NULL;
    p->state = RUNNABLE;
    return ;
}

void fork_ret()
{
    prepare_return();
    PROC* p = myproc();
    uint32 satp = p->pagetable ;
    userret(satp);
}

// freeproc() - free all pages occupied by the proc , free its trapframe , free the proc structure 
// proc related syscalls 
// sleep()
// wait()
// exit()
// yeild()

