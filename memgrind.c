#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include <sys/time.h>
#include "mymalloc.h"
#define MEAN_TIME(start,stop) ((stop.tv_sec - start.tv_sec) * 1000000 +\
                                stop.tv_usec - start.tv_usec) / 1.e6


//gcc mymalloc.c memgrind.c

void workloadA();
void workloadB();
void workloadC();
void workloadD();
void workloadE();
void shuffleArray(char**, int);
void testTime();
void fragmentingTest();

int main(int argc,char* argv[])
{   

    printf("Testing basic cases:\n");
    printf("--------------------\n");
    printf("A: Free()ing addresses that are not pointers:\n");
    int x;
    //free((int*) x);
    
    printf("\n\n");
    printf("B: Free()ing pointers that were not allocated by malloc():\n");
    char* p;
    p = (char *)malloc( 200 );
    free(p+10);
    int *k; 
    free(k);

    printf("\n\n");
    printf("C:Redundant free()ing of the same pointer:\n");
    p = (char*)malloc(100);
    free(p); 
    free(p);

    p = (char *)malloc( 100 );
    free(p);
    p = (char *)malloc( 100 ); 
    free(p);

    printf("\n\n");
    printf("D: Saturation of dynamic memory:\n");
    p = (char*)malloc(5000);

    printf("-------------------------------\n\n");
    printf("Workloads:\n");
    testTime();
    return 0;
}


/*
 * Run all the workloads, one after the other, 50 times.
 * Do like Workload A,B,C,D,E.........A,B,C,D,E in cycle
 * And  record the runtime for each workload and store it
 */
void testTime()
{
    struct timeval start,stop;
    double timeArr[5];
    gettimeofday(&start, NULL);
    for(int i = 0;i<50;i++)
    {
        gettimeofday(&start, NULL);
        workloadA();
        gettimeofday(&stop, NULL);
        timeArr[0]+= MEAN_TIME(start,stop);

        gettimeofday(&start, NULL);
        workloadB();
        gettimeofday(&stop, NULL);
        timeArr[1]+= MEAN_TIME(start,stop);

        gettimeofday(&start, NULL);
        workloadC();
        gettimeofday(&stop, NULL);
        timeArr[2]+= MEAN_TIME(start,stop);

        gettimeofday(&start, NULL);
        workloadD();
        gettimeofday(&stop, NULL);
        timeArr[3]+= MEAN_TIME(start,stop);

        gettimeofday(&start, NULL);
        workloadE();
        gettimeofday(&stop, NULL);
        timeArr[4]+= MEAN_TIME(start,stop);
    }
    for(int i = 0;i<5;i++)
    {
        printf("Time for Test %c: %f (seconds)\n",'A'+i,timeArr[i]);
    }

}
/* Workload A                                                  */
/* malloc() 1 byte and immediately free it - do this 120 times */
void workloadA()
{

    char* p;
    for(int i = 0;i<120;i++)
    {
       p = (char*)malloc(1);
       free(p);
    }
}
/* Workload B                                                          */
/* malloc() 1 byte, store the pointer in an array - do this 120 times. */
void workloadB()
{
    char* arr[120];
    for(int i = 0;i<120;i++)
    {
       arr[i] =(char*) malloc(1);
       //printf("%d Addr: %d\n",i,(int)arr[i]);
    }
    for(int i = 0;i<120;i++)
    {
       free(arr[i]);
       //printf("%d Addr: %d\n",i,(int)arr[i]);
    }
}
/* Workload C                                             */
/*  Randomly choose between a 1 byte malloc()             */
/*  or free()ing one of the malloc()ed pointers 240 times */
void workloadC()
{
    char* arr[120];
    int random;
    int i = 0;
    int j = 0;
    while (i<120)
    {
        random = rand()%2;
        if (random == 0)
        {
            arr[j] = (char*)malloc(1);
            i++;
            j++;
        }
        else if(random == 1 && j > 0)
        {
            free(arr[j-1]);
            j--;
        }    
    }
    while(j>0)
    {
        free(arr[j-1]);
        j--;
    }
}

/*
 * Workload D
 * In order to test fragmenting memory,
 * this function will malloc 30 bytes pointers until the main memory is full,
 * randomly free 10 of them, and request a block smaller than the total memory 
 * but largers than any free block.
 */
void workloadD()
{
    int random;
 
    char* arr[200];
    int i = 0;
    int keepmalloc = 1;
    
    while(keepmalloc && i<200)
    {
        arr[i]=(char*)malloc(30);
        if(arr[i]==NULL)
        {
            keepmalloc = 0;
            break;
        }
        i++;
    }
    shuffleArray(arr,i);

    int j;
    for(j = 0;j<10;j++)
    {
        free(arr[j]);
    }

    char* p = malloc(200);
    char* p2 = malloc(500);

    for(;j<i;j++)
    {
        free(arr[j]);
    }    
}

/*
 * Workload E
 * This function will randomly pointers between 1 and 100 bytes, 
 * and randomly free them to test if free() merge free block successfully. 
 *
 */
void workloadE()
{
    char* arr[30];
    int i = 0;
    int random;

    for(int i = 0;i<30;i++)
    {
        random = rand() % 100;
        arr[i] =(char*)malloc(random);
        //printf("%d Malloc(%d) addr: %d\n",i,random,(int)arr[i]);
    }

    shuffleArray(arr,30);

    for(int i = 0;i<30;i++)
    {
        //printf("%d Free addr: %d\n",i,(int)arr[i]);
        free(arr[i]);
    }
}

void shuffleArray(char* array[],int size)
{
    int index; 
    char* temp;
    int random;
    srand ( time(NULL) ); 
    for (int i = size -1; i > 0; i--)
    {
        int index = rand()%(size-1);
        temp = array[index];
        array[index] = array[i];
        array[i] = temp;
    }
}
