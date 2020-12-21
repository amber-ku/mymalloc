#ifndef _MYMALLOC_H
#define _MYMALLOC_H
#define TOTAL_MEMORY 4096
#define NEXT(b) (BLOCK*)((char*)(b+1)+b->size)
#define malloc(x) mymalloc((x), __FILE__, __LINE__)
#define free(x) myfree((x), __FILE__, __LINE__)
#define MAX_MEM(x) (size_t)(x+TOTAL_MEMORY-1)
#define TRACK(msg,p) printf("%-15s Size: %-6lu, Free: %d, Addr: %d\n",msg,p->size,p->free,(int)p);


#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>


#define ALLOCERROR(x,file,line) printf("Error: %s in file: \"%s\" line: #%u\n",x,file,line)
#define FREE_ERROR(x,file,line) printf("Error: %s in file: \"%s\" line: #%u\n",x,file,line)



typedef struct block{
    size_t size;
    int free;
    struct block* prev;
}BLOCK;


void* mymalloc(size_t, char *, unsigned int);
void myfree(void* ,char * ,unsigned int);
void split(BLOCK* ,size_t );
void merge(BLOCK*);
void initialize();
BLOCK* search(size_t );

#endif