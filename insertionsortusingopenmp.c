#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 50000
#define NUM_THREADS 3

int unsorted[SIZE];
int sorted[SIZE];

struct merge {
    int begin;
    int mid;
    int end;
};

struct insertionSort {
    int start;
    int end;
};

void fillArray() {
    int i,n;
    srand(time(NULL));
    for (i=0; i<SIZE; i++)	
    {	 
     unsorted[i] = rand() % 100;
 }
}


void* merge() 
{

    int start1 = 0,
    end1 = SIZE/2,
    start2 = end1+1,
    end2 = SIZE-1;
    
    int begin = start1,
    mid = start2,
    end = end2;


    int i = begin, j = mid, tpos = begin;

    while (i < mid && j <= end)
    {
        if (unsorted[i] < unsorted[j])
            sorted[tpos++] = unsorted[i++];
        else
            sorted[tpos++] = unsorted[j++];
    }

    //still elements left over in first list. copy over
    while (i < mid)
        sorted[tpos++] = unsorted[i++];

    //still elements left over in first list. copy over
    while (j <= end)
        sorted[tpos++] = unsorted[j++];
    return NULL;
}

/**
 * Insertion sort
 * Two parameters used according to threads	
 * First Thread's parameter : index of the first element in unsorted sublist
 * First Thread's parameter : index of the last element in unsorted sublist
 */

void* InsertionSort(int k)
{

    int start1 = 0,
    end1 = SIZE/2,
    start2 = end1+1,
    end2 = SIZE-1;
    int start,end;
       if(k == 1){
    start = start1;
    end = end1;
}
else{
    start = start2;
    end = end2;
}


    int i = start, j, itemToInsert;

    //everything to the left of i is sorted
    while (i <= end)
    {
        itemToInsert = unsorted[i]; //a must, or else unsorted[i] gets overwritten when shifting elements

        //since everything in this sequence is sorted,starting from i, and going in reverse order, shift the elements to the right until an element less than unsorted[i] is found
        j = i-1;
        while (j >= start && itemToInsert < unsorted[j])
        {
            unsorted[j+1] = unsorted[j];
            j--;
        }
        //insert the element into s[j]
        unsorted[j+1] = itemToInsert;
        i++;
    }
    return NULL;
}

void printArray(int *list, int size) {
    int i;
    for (i=0; i<size; i++)
        printf("%d, ", list[i]);
}

int main() {

   printf("\nUnSorted Array : \n");
    fillArray();   
    printArray(unsorted, SIZE);
   
    int start1 = 0,
    end1 = SIZE/2,
    start2 = end1+1,
    end2 = SIZE-1;
    
    clock_t startingTime, endingTime;
   

    startingTime=clock();

int i;

#pragma omp parallel private(i) num_threads(3)
{
#pragma omp for
for(i=1;i<=2;i++){
InsertionSort(i);
}

#pragma omp barrier    
merge();
}
    endingTime =clock();	
    
    printf("\nSorted Array : \n");
    printArray(sorted, SIZE);
    printf("\nTime taken for execution: %f seconds\n", (endingTime -startingTime)/(double)CLOCKS_PER_SEC);
   return 0;
}
