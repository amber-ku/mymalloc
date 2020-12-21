#include "mymalloc.h"

static char myblock[TOTAL_MEMORY];
BLOCK* free_list;

/*
 * Function: mymalloc
 * ----------------------------
 *  This function checks if the current memory space 
 *  is large enough to allocate memory requested. 
 *  If it has enough space, it returns void* point to the free memory block, 
 *  else it prints an error message indicating the error. 
 *
 *   size: the memory size user requested
 *   *file: indicate the error in which file
 *   line: indicate error in which line
 *
 *   returns: void* pointer if the memory allocation is successful
 *            NULL if the allocation fails
 */

void* mymalloc(size_t size, char *file, unsigned int line)
{

    if(free_list == NULL)
        initialize();

    //check if the memory requested is valid
    if (size > TOTAL_MEMORY-sizeof(BLOCK))
    { 
        ALLOCERROR("memory requested is too large", file, line);
        return NULL;
    }

    BLOCK* temp;
    temp = search(size);

    //found the available block, split it if necessary
    if(temp != NULL)
    {
        if((temp->size - size) >= (sizeof(BLOCK)+1))
        {
            split(temp,size);
        }
        return (void*)(temp+1);
    }
    //Current requested memory don't fit current free blocks 
    else 
    {
        ALLOCERROR("no available memory", file, line);
        return NULL;   
    }

}


/*
 * Function: myfree
 * ----------------------------
 *  This function checks if the parm pointer points the valid address,
 *  if not, it prints error message.
 *  If the parm pointer points to a allocated BLOCK, 
 *  this function switchs the block into free,
 *  calls merge function merging adjacent free blocks.
 *
 *   p: the pointer going to be free()ed
 *   *file: indicate the error in which file
 *   line: indicate error in which line
 */

void myfree(void* p,char *file, unsigned int line)
{

    //p is NULL
    if(p == NULL)
    {
        FREE_ERROR("attempt to free NULL pointer", file, line);
        return;
    }

    //p points to the address out of myblock
    if((size_t)p > MAX_MEM(myblock) || (size_t)p<(size_t)myblock)
    {
        FREE_ERROR("attempt to free invalid address", file, line);
        return;
    }

    BLOCK* temp = (BLOCK*)((char*)p-sizeof(BLOCK));

    // Free()ing pointers that were not allocated by malloc()
    if(temp->prev == NULL && temp != (BLOCK*)myblock)
    {
        FREE_ERROR("attempt to free illegal memory address", file, line);
        return;
    }

    //Free()ing the valid pointer
    if(temp->free == 0)
    {
        //TRACK("\n\nBefore free:\nCurrent: ",temp);
        temp->free = 1;
        merge(temp);
        //TRACK("Current: ",temp);
        return;
    }
    //Free()ing a free block
    else if(temp->free == 1 && temp->size > 0)
    {
        FREE_ERROR("attempt to free unallocated memory", file, line);
        return;
    }
}


/*
 * Function: split
 * ----------------------------
 *  This function split the current block into two blocks
 *
 *   b: the BLOCK going to be splitted
 *   size: the size is requseted by user, will be the size of the previous block
 */

void split(BLOCK* b,size_t size)
{
    BLOCK* new;
    new = (BLOCK*)((char*)(b + 1)+size);
    new->size = b->size - size - sizeof(BLOCK);
    new->free = 1;
    new->prev = b;
    b->size = size;
    b->free = 0;
    //TRACK("Allocated:",b);
    //TRACK("New:",new);
}


/*
 * Function: merge
 * ----------------------------
 *  This function merge the adjacent free blocks.
 *
 *   curr: the BLOCK going to be merged if the adjacent is free
 */

void merge(BLOCK* curr)
{
    BLOCK* next = NEXT(curr);
    BLOCK* nextNext = NEXT(next);

    //next is free
    if(next->free == 1)
    {
        curr->size += next->size+sizeof(BLOCK);
        if(nextNext->prev!=NULL)
            nextNext->prev=curr;
        
        //next is free and prev is free
        if(curr->prev != NULL)
        {
            if(curr->prev->free == 1)
            {
                curr->prev->size += curr->size+sizeof(BLOCK);
                //TRACK("After Merging\nPrevious:",curr->prev);
                nextNext->prev = curr->prev;
            }
        } 
        return;
    }

    //next is not free but prev is free
    if(curr->prev!=NULL)
    {
        if(curr->prev->free == 1)
        {
            curr->prev->size += curr->size+sizeof(BLOCK);
            //TRACK("After Merging\nPrevious:",curr->prev);
            next->prev = curr->prev;
        }
    }
}


/*
 * Function: search
 * ----------------------------
 *  This function search a free block from the beginning of myblock
 *
 *   size: the size is requseted by user
 */

BLOCK* search(size_t size)
{   
    //printf("\n\nSize Requested: %lu\n",size);
    BLOCK* b =(BLOCK*)myblock;
    while((size_t)b<MAX_MEM(myblock))
    {
        if(b->free==1&&b->size>=size)
        {
            //TRACK("Found:",b);
            return b;
        }
       
        b = NEXT(b);
        //printf("Size: %lu, Free: %d, Addr: %d\n",b->size,b->free,(char)b);
    }
    return NULL; 
}

/*
 * Function: initialize
 * ----------------------------
 *  This function initilizes the free list
 */

void initialize()
{
    free_list = (BLOCK*)myblock;
    free_list->size = TOTAL_MEMORY - sizeof(BLOCK);
    free_list->free = 1;
    free_list->prev = NULL;
}









