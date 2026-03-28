#include "heap.h"
#include "syscall.h"


HEAP_HEADER* page = NULL;
/*
    Heap mem allocation flow 
                                                  if not first   
    user calls malloc -> malloc checks is first ---------------> then search for a hole in the free list ------------------------------ |----> if hole is not found then make syscall to kernel to alloc more mem -> add the new mem as hole in the free list --> consume and return base addr to user
                            |                                                                                                           |
                            | if yes                                                                                                    | if hole is found ( alloc mem req by user and alloc mem header )
                            make syscall to alloc heap memory                                                                           |
                            initialize the heap header                                                                                  | consume the hole ---> remove the hole from the free list ---> return base address 
                            create the initial hole which is mem allocated - ( head header - mem req by user)                           |    
                            insert the allocated heap header into the allocated mem 
                            return base addr to the user 

*/

#define MAGIC_HOLE 0x484f4c45   //HOLE


void break_point(void* param)
{
    __asm__ volatile (
        "mv a0, %0\n"
        :
        : "r"(param)
        : "a0"
    );

    while(param != -1 * 0x100);

    return ;
}

uint32 roundup(uint32 val, uint32 multiple)
{
    if (multiple == 0) return val;  // avoid division by zero

    return ((val + multiple - 1) / multiple) * multiple;
}


void heap_init()
{   
   // first hole is going to be page size - 4 ( the heap header will consume 4 bytes )
   page->head_32b = (HOLE_32B*) ( page + 1 );
   page->head_32b->base_addr = (uint32*) (page + 1);
   page->head_32b->size = PAGE_SIZE - sizeof(HEAP_HEADER);
   page->head_32b->left = NULL;
   page->head_32b->right = NULL;

   // set all the elements in the array to NULL at the start
   page->array_of_head_l32b[0] = NULL;
   page->array_of_head_l32b[1] = NULL;
   page->array_of_head_l32b[2] = NULL;
   page->array_of_head_l32b[3] = NULL;
   page->array_of_head_l32b[4] = NULL;
   page->array_of_head_l32b[5] = NULL;
   page->array_of_head_l32b[6] = NULL;
}



void add_hole(uint32 size , uint32* base_addr)
{
    // traverse the tree for insertion
    if(size >= 32)
    {
        // traverse the tree to find the node of insertion 
        HOLE_32B* tmp = page->head_32b;
        int is_right = -1;
        while(tmp != NULL)
        {
            if(size > tmp->size)
            {
                if(tmp->right != NULL)
                    tmp = tmp->right;
                else
                {
                    is_right = 1;
                    break;
                }
            }
            else
            {
                if(tmp->left != NULL)
                    tmp = tmp->left;
                else
                {
                    is_right = 0;  // left
                    break;
                }
            }
        }

        // insert the node at base_addr
        HOLE_32B* ptr = (HOLE_32B*)base_addr;
        ptr->base_addr = base_addr;
        ptr->size = size;
        ptr->left = NULL;
        ptr->right = NULL;

        break_point(is_right);
        if(is_right == 1)
        {
            // insert the node at the right of tmp
            tmp->right = ptr;
        }
        else if(is_right == 0)
        {
            // insert the node to the left of tmp
            tmp->left = ptr;
        }
        else
        {
            // you are inserting head node
            page->head_32b = ptr;
        }
    }
    else
    {
        // size of the whole can be either of these 4 8 12 16 20 24 28


        HOLE_L32B* tmp;
        uint32 index = ( size / 4 - 1) ;
        tmp = page->array_of_head_l32b[index];

        // inserting the fist node 
        if(tmp == NULL)
        {
            page->array_of_head_l32b[index] = (HOLE_L32B*) base_addr;
            tmp = (HOLE_L32B*)base_addr;
            tmp->next = NULL;
            return ;
        }

        // traverse the linked list 
        while(tmp->next != NULL) { tmp = tmp->next ;}

        // add the node to the linked list
        HOLE_L32B* ptr = (HOLE_L32B*) base_addr;
        ptr->next = NULL;
        tmp->next = ptr;
    }

}


HOLE_32B* search_parent(HOLE_32B* hole)
{
    uint32 size = hole->size;
    uint32 current = 0;

    HOLE_32B* parent = NULL;
    HOLE_32B* tmp = page->head_32b;


    // traverse the tree 

    while(1)
    {
        current = tmp->size;
        if(current >= size)
        {
            // take the left branch
            
            if(tmp->left != NULL)
            {
                parent = tmp;
                tmp = tmp->left;
            }
            else
            {
                break;
            }
            
        }
        else
        {
            
            if(tmp->right != NULL)
            {
                parent = tmp;
                tmp = tmp->right;
            }
            else
            {
                break;
            }           
        }
    }
    if(tmp == hole)
    {
        // check if the whole was found
        return parent;
    }
    else
    {
        return NULL;
    }
    

}



HOLE_32B* search_for_hole(uint32 size)
{
    uint32 current;
    HOLE_32B* prev_highest = NULL;

    HOLE_32B* tmp = page->head_32b;


    // traverse the tree 

    while(1)
    {
        current = tmp->size;
        if(current >= size)
        {
            // take the left branch
            prev_highest = tmp;
            if(tmp->left != NULL)
            {
                tmp = tmp->left;
            }
            else
            {
                break;
            }
        }
        else
        {

            if(tmp->right != NULL)
            {
                tmp = tmp->right;
            }
            else
            {
                break;
            }
           
        }
    }

    
    if(tmp->size != size)
    {
        return prev_highest;
    }
    else if(prev_highest == NULL)
    {
        return NULL;
    }
    else
    {
        // exact match
        return tmp;
    }
}


int remove_hole_32B(HOLE_32B* hole)
{

    if(hole == NULL)
        return -1;
    HOLE_32B* left = hole->left;
    HOLE_32B* right = hole->right;

    // search for the replacing node
    HOLE_32B* replacing_hole = NULL;

    if(right != NULL)
    {
        replacing_hole = right;
        while(replacing_hole->left != NULL)
        {
            replacing_hole = replacing_hole->left;
        }
    }
    else if(left != NULL)
    {
        replacing_hole = left;
        while(replacing_hole->right != NULL)
        {
            replacing_hole = replacing_hole->right;
        }
    }
    else
    {
        // leaf node or parent node 
        HOLE_32B* deleting_node_parent = search_parent(hole);

    
        if(deleting_node_parent == NULL)
        {
            // parent node
            page->head_32b = NULL;
        }
        else if(deleting_node_parent->right == hole)
        {
            deleting_node_parent->right = NULL;
        }
        else
        {
            deleting_node_parent->left = NULL;
        }
    }

    // deleted hole was a leaf or parent node so no replacing node
    if(replacing_hole == NULL)
        return 0;

    HOLE_32B* replacing_node_parent = search_parent(replacing_hole);
    if(replacing_node_parent == NULL)
    {
        return -1;
    }

    // replace the hole by the replacing_hole
    hole->size = replacing_hole->size;
    hole->base_addr = replacing_hole->base_addr;

    // update the replacing hole's parent
    if(replacing_hole->right == NULL && replacing_hole->left == NULL)
    {
        if(replacing_node_parent->right == replacing_hole)
        {
            replacing_node_parent->right == NULL;
        }
        else
        {
            replacing_node_parent->left = NULL;
        }
    }
    else if(replacing_hole->right == NULL)
    {
        replacing_node_parent->right = replacing_hole->left;
    }
    else
    {
        replacing_node_parent->left = replacing_hole->right;
    }
  
    return 0;
}

int remove_hole_l32B(uint32 size)
{
    uint32 index = (size / 4) - 1;
    HOLE_L32B* head = page->array_of_head_l32b[index];

    if(head == NULL)
    {
        // list is empty hole cannot be removed 
        return -1;
    }

    // replace the head node with the node next to head
    page->array_of_head_l32b[index] = head->next;

    return 0;

}


// function to allocate mem when a hole is found , it consumes the older hole and will attach the remaining hole back to the free list 
// the size should be a multiple of 4 
void* allocate_mem(HOLE_32B* hole, uint32 size)
{
    // remove the hole from the list
    uint32 prev_size = hole->size;
    if(remove_hole_32B(hole) == -1)
        return NULL;

    // allocate the mem by inserting the mem_allocation_header
    MEM_ALLOCATION_HEADER* header = (MEM_ALLOCATION_HEADER*)hole;
    header->magic = MAGIC_HOLE;
    header->size = size;

    // insert the new hole ( prev hole - size ) into the free list 
    uint32 new_size = prev_size - size;
    if(new_size >= 4)
    {
        add_hole(new_size, (uint32*)((char*)hole + size));
    }

    // return the base address of the hole 
    break_point((void*)header + sizeof(MEM_ALLOCATION_HEADER));
    return header + sizeof(MEM_ALLOCATION_HEADER);
}

void* malloc(uint32 size)
{
    static int isfirst = 1;

    // round up size to multiple for 4 hole
    size = roundup(size,4);
    HOLE_32B* hole = NULL;
    void* base_addr = NULL;

    if(isfirst)
    {
        isfirst = 0;

        // initialise the heap header
        page = (HEAP_HEADER*)sys_sbrk(size);

        heap_init();
        
        hole = search_for_hole(size + sizeof(MEM_ALLOCATION_HEADER));
        base_addr = allocate_mem(hole, size);
        break_point(page);
        return base_addr;
    }

    hole = search_for_hole(size + sizeof(MEM_ALLOCATION_HEADER));
    if(hole == NULL)
    {
        // allocate more mem from kernel
        HOLE_32B* hole = (HOLE_32B*)sys_sbrk(size);

        // sys_brk will always allocate mem in multiples of page size so update size in hole structure

        uint32 size_allocated =roundup(size,PAGE_SIZE);
 
        // construct the hole header
        hole->base_addr = (uint32*)hole;
        hole->size = size_allocated;
        hole->left = hole->right = NULL;
        
    }

    // allocate the requested memory and add the unconsumed memory back to the tree 
    base_addr = allocate_mem(hole,size);

    return base_addr;
}