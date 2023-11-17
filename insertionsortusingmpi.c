#include<mpi.h>
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// size of array
#define n 50000
int a[n];



void fillArray() {
    int i;
    srand(time(NULL));
    for (i=0; i<n; i++)	
    {	 
     a[i] = rand() % 100;
 }
}




//*********************************************************************  
/**
 * Insertion sort
 * Two parameters used according to threads	
 * First Thread's parameter : index of the first element in unsorted sublist
 * First Thread's parameter : index of the last element in unsorted sublist
 */
/* Function to sort an array using insertion sort*/
//i is the index of array
void insertionSort(int arr[],int n1){
int  i,key, j;
for (i=0; i < n1; i++){
key = arr[i];
//printf("key %d\n",key);
j = i - 1;
/* Move elements of arr[0..i-1], that are
greater than key, to one position ahead
of their current position */
while (j >= 0 && arr[j] > key) {
arr[j + 1] = arr[j];
j = j - 1;
}
arr[j + 1] = key;
}
}

// A utility function to print an array of size n
void printArray(int arr[], int n1){
int i;
for (i=0; i < n1; i++){
printf("%d ", arr[i]);
printf("\n");
}
}
/* Driver program to test insertion sort */
//*****************************************************************************************************************************************  
// Temporary array for slave process
int a2[50000];
int main(int argc, char* argv[])
{
int pid,np,elements_per_process,n_elements_recieved;
MPI_Status status;
// Creation of parallel processes
MPI_Init(&argc, &argv);
// find out process ID,
// and how many processes were started
MPI_Comm_rank(MPI_COMM_WORLD, &pid);
MPI_Comm_size(MPI_COMM_WORLD, &np);
// master process
if (pid == 0){

clock_t startingTime, endingTime;
startingTime=clock();

fillArray();   
printf("\nUnSorted Array : \n");
int g;
for(g=0;g<n;g++){
printf("%d\n",a[g]);
}



int index, i;
elements_per_process = n/np;
// check if more than 1 processes are run
if (np > 1) {
// distributes the portion of array
// to child processes to calculate
// their partial sums
for (i =1; i < np-1; i++){
index = i * elements_per_process;
MPI_Send(&elements_per_process,1,MPI_INT,i,0,MPI_COMM_WORLD);
MPI_Send(&a[index],elements_per_process,MPI_INT,i,0,MPI_COMM_WORLD);
}

// last process adds remaining elements
index = i * elements_per_process;
int elements_left = n - index;
MPI_Send(&elements_left,1,MPI_INT,i,0,MPI_COMM_WORLD);
MPI_Send(&a[index],elements_left,MPI_INT,i,0,MPI_COMM_WORLD);
}

insertionSort(a,elements_per_process);
int tmp[n],sorted_array[n],n_elements;
int j;
int k=0;
for(j=0;j<elements_per_process;j++){
sorted_array[j]=a[j];
//printf("\ntemp1 %d\n",sorted_array[j]);
}

for(i = 1; i < np; i++){	
MPI_Recv(&n_elements,1,MPI_INT, MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
MPI_Recv(&tmp,n_elements, MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
int sender = status.MPI_SOURCE;
//printf("temp %d:\n",tmp[4]);

for(k=0;k<n_elements;k++){

sorted_array[j]=tmp[k];
//printf("\ntemp %d\n",sorted_array[j]);
j++;
}
}

//printf("j %d",j);
int n_size=sizeof(sorted_array)/sizeof(int);
//printf("n_size %d\n",n_size);
insertionSort(sorted_array,n_size);
// prints the final sum of array
printf("sorted array is :\n");
for(i=0;i<n_size;i++){
printf("%d ,",sorted_array[i]);
}

    endingTime =clock();	
    printf("\nTime taken for execution: %f seconds\n", (endingTime -startingTime)/(double)CLOCKS_PER_SEC);

}

// slave processes
else {
MPI_Recv(&n_elements_recieved,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
// stores the received array segment
// in local array a2
MPI_Recv(&a2, n_elements_recieved,MPI_INT,0,0,MPI_COMM_WORLD,&status);
insertionSort(a2,n_elements_recieved);
//printf("a2 %d %d %d %d %d %d %d\n",a2[0],a2[1],a2[2],a2[3],a2[4],a2[5],a2[6]);
//printf("\na2 size %d\n",n_elements_recieved);
MPI_Send(&n_elements_recieved,1,MPI_INT,0,0,MPI_COMM_WORLD);
// sends the partial sum to the root process
MPI_Send(&a2, n_elements_recieved, MPI_INT,0,0,MPI_COMM_WORLD);
}
// cleans up all MPI state before exit of process
MPI_Finalize();
return 0;
}

