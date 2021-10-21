//mpicc filename.c
//mpirun -n p ./a.out 
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
void sort(int *arr, int n) {
	int i, j;
	for (i = 1; i < n; i++)
		for (j = 0; j < n - i; j++)
			if (arr[j] > arr[j + 1]) {
				int tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}// end if
}
int* concat(int *a, int len1, int *b, int len2)
{
	int *cat=malloc ((len1+len2)* sizeof (int));
		for (int i = 0; i < len1; i++)
			cat[i]=a[i];
		for (int i = 0; i < len2; i++)
			cat[len1+i]=b[i];
		return cat;
}
int main(int argc, char *argv[]) {

	int myid, procs, err, i, j;
	MPI_Status status;
	srand(time(NULL));
	// initialize MPI_Init
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}// end if
	MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	int n=20;
	int range=20;
	int *arr = malloc(n * sizeof(int));
	if (myid == 0) 
	{	
		srand(time(0));
		for (int i = 0; i < n; i++) {
			arr[i] = rand() % range;
			printf("%d ", arr[i]);
		}
		printf("\n");
		for (int i = 1; i < procs; i++) {
			MPI_Send(arr, n, MPI_INT, i, myid, MPI_COMM_WORLD);
		}
		int *sorted=malloc(n * sizeof(int));
		int slen=0;
		for (int i = 1; i < procs; i++)
		{
			int len;
			MPI_Recv(&len, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			int *bucket=malloc(len * sizeof(int));
			MPI_Recv(bucket, len, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			sorted=concat(sorted,slen,bucket,len);
			slen+=len;
		}
			
		printf("\nSorted array:\n");
		for (int i = 0; i < n; i++) {
			printf("%3d", sorted[i]);
		}
	}
	else 
	{
		int start, end, k = range / (procs - 1);
		start = (myid - 1) * k;
		end = start + k;
		int *tempbucket = malloc(n * sizeof(int));
		int count=0;
		MPI_Recv(arr, n, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		for (int i = 0; i < n; i++) 
		{
			if ((start <= arr[i]) && (arr[i] < end))
				tempbucket[count++] = arr[i];
		}
		int *bucket = malloc(count * sizeof(int));
		for (int i = 0; i < count; i++) {
			bucket[i] = tempbucket[i];
		}
		sort(bucket, count);
		MPI_Send(&count, 1, MPI_INT, 0, myid, MPI_COMM_WORLD);	
		MPI_Send(bucket, count, MPI_INT, 0, myid, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}