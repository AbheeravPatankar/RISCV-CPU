#include "heap.h"

HEAP_HEADER* page = NULL;


void heap_init(uint32 size, uint32* base_addr)
{   
   // first hole is going to be page size - 4 ( the heap header will consume 4 bytes )
   page->head_4b = (HOLE_4B*) ( page + 1 );
   page->head_4b->base_addr = (uint32*) (page + 1);
   page->head_4b->size = PAGE_SIZE - sizeof(HEAP_HEADER);
   page->head_4b->left = NULL;
   page->head_4b->right = NULL;

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
        HOLE_4B* tmp = page->head_4b;
        int is_right = 0;
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
                    break;
            }
        }

        // insert the node at base_addr
        HOLE_4B* ptr = (HOLE_4B*)base_addr;
        ptr->base_addr = base_addr;
        ptr->size = size;
        ptr->left = NULL;
        ptr->right = NULL;

        if(is_right)
        {
            // insert the node at the right of tmp
            tmp->right = ptr;
        }
        else
        {
            // insert the node to the left of tmp
            tmp->left = ptr;
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


HOLE_4B* search_parent(HOLE_4B* hole)
{
    uint32 size = hole->size;
    uint32 current = 0;

    HOLE_4B* parent = NULL;
    HOLE_4B* tmp = page->head_4b;


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



HOLE_4B* search_for_hole(uint32 size)
{
    uint32 current;
    HOLE_4B* prev_highest = NULL;

    HOLE_4B* tmp = page->head_4b;


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


int remove_hole_32B(HOLE_4B* hole)
{

    if(hole == NULL)
        return -1;
    HOLE_4B* left = hole->left;
    HOLE_4B* right = hole->right;

    // search for the replacing node
    HOLE_4B* replacing_hole = NULL;

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
        // leaf node
        HOLE_4B* deleting_node_parent = search_parent(hole);
        if(deleting_node_parent->right == hole)
        {
            deleting_node_parent->right = NULL;
        }
        else
        {
            deleting_node_parent->left = NULL;
        }
    }

    // deleted hole was a leaf node so no replacing node
    if(replacing_hole == NULL)
        return 0;

    HOLE_4B* replacing_node_parent = search_parent(replacing_hole);
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
