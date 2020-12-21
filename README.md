# mymalloc
This is Assignment1 in CS 214:System Programming, Fall 2020 \
Completed by Hsinghui Ku & Chang Li
## Description
In this assignment, we implement malloc() and free() library calls for dynamic memory
allocation that detect common programming and usage errors.
 Malloc( size_t size ) is a system library call that returns a pointer to a block of memory of
at least the requested size. This memory comes from a main memory resource managed by the
operating system. The free( void * ) function informs the operating system that you are done with a
given block of dynamically-allocated memory, and that it can reclaim it for other uses. 
We use a large array to simulate main memory (static char myblock[4096]). Our malloc()
function will return pointers to this large array and your free() function will let your code know that a
previously-allocated region can be reclaimed and used for other purposes. 

## Detectable Errors
Our malloc is able to to catch the following errors
#### Free()ing addresses that are not pointers:
```C
int x;
free( (int*)x );
```
#### Free()ing pointers that were not allocated by malloc():
```C
p = (char *)malloc( 200 );
free( p + 10 );
- or -
int * x;
free( x );
```
#### Redundant free()ing of the same pointer:
```C
//This is an error example
p = (char*)malloc(100);
free( p );
free( p );

//This is valid
p = (char *)malloc( 100 );
free( p );
p = (char *)malloc( 100 );
free( p );
```
#### Saturation of dynamic memory:
```C
p = (char*)malloc(5000);
- or -
p = (char*)malloc(4096);
q = (char*)malloc(1);
```

## Responding to Detected Errora
Our modified malloc() and free() will report the precise calls that caused dynamic memory
problems during program execution. 

## Metadata Design
This program has following struct as metadata:
```C
typedef struct block
{ 
    size_t size;
    int free;
    struct block* prev;
}BLOCK;
       
/* size_t size​: indicates the size of memory requested, this size doesn't include the size of metadata. */
/* int free​:​ indicates if the current block is free or not. */
/* struct block*​ ​prev​: points to previous block. */
```

