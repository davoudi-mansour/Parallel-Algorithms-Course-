//mpicc filename.c
//mpirun -n p ./a.out 
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#define MAX 500 //define the array size
int* subarray(int *array, int start, int len)
{
	int *temp=(int*) malloc(len*sizeof(int));
	int i,k=0;
	for (i=start;i<start+len;i++,k++)
		temp[k]=array[i];
	return temp;
}
void sort (int *a, int len)
{
	int i,j,temp;
	for (i=0;i<len;i++)
		for (j=0;j<len-i-1;j++)
			if (a[j]>a[j+1])
			{
				temp=a[j];
				a[j]=a[j+1];
				a[j+1]=temp;
			}
		
}
int* merge (int *a,int *b, int len)
{
	int *mg=(int*) malloc(2*len*sizeof(int));
	int i=0;
	int j=0;
	int k=0;
	while ((i<len )&&(j<len))
		{
			if (a[i]<b[j])
				{
					mg[k]=a[i];i++;k++;
				}
			if (a[i]>=b[j])
			{
				mg[k]=b[j];k++;j++;
			}
		}
		while (i<len)
			{mg[k]=a[i];k++;i++;}
		while(j<len)
			{mg[k]=b[j];k++;j++;}
	return mg;
}
int* part1(int *array, int len)
{
	int i,*temp=(int*) malloc(len/2*sizeof(int));
	for (i=0;i<len/2;i++)
		temp[i]=array[i];
	return temp;
}
int* part2(int *array, int len)
{
	int *temp=(int*) malloc(len/2*sizeof(int));
	int i,k=0;
	for (i=len/2;i<len;i++,k++)
		temp[k]=array[i];
	return temp;
}
int main(int argc, char *argv[]) {
	srand(time(NULL));
	int myid, procs, err, i, j,h,k, type;
	MPI_Status status;
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}// end if
	MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	int *value=(int*) malloc(k*sizeof(int));
    int n=20;
	k=n/procs;
	int y,x;
	int *temp=(int*) malloc(k*sizeof(int));
	int *merged=(int*) malloc(2*k*sizeof(int));
	if (myid==0)
	{		
		int *array=(int*) malloc(n*sizeof(int));
		int *total=(int*) malloc(n*sizeof(int));
		for( i=0;i<n;i++)
			array[i] = rand()%100;
		for( i=0;i<n;i++)
			printf("%d ",array[i]);
		printf("\n");
		value=subarray(array,0,k);
		sort(value,k);

		for(i=1;i<procs;i++)
		{
			int start=i*k;
			temp=subarray(array,start,k);
			sort(temp,k);
			merged=merge (value,temp,k);
			value=part1(merged,2*k);
			temp=part2(merged,2*k);
			MPI_Send(temp, k, MPI_INT, 1, 0, MPI_COMM_WORLD);
		}
		for( h=0;h<k;h++)
			total[h]=value[h];
		for(j=1;j<procs;j++)
		{
			MPI_Recv(temp, k, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			for( h=0;h<k;h++)
				total[j*k+h]=temp[h];
		}
		printf(" \nSorted:\n");
		for( j=0;j<n;j++)
			printf("%d ",total[j]);
	}
	if (myid!=0)
	{
		MPI_Recv(value, k, MPI_INT, myid-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		for(i=0;i<procs-myid-1;i++)
		{
			MPI_Recv(temp, k, MPI_INT, myid-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			merged=merge(value,temp,k);
			value=part1(merged,2*k);
			temp=part2(merged,2*k);
			MPI_Send(temp, k, MPI_INT, myid+1, myid, MPI_COMM_WORLD);
		}
		MPI_Send(value, k, MPI_INT, 0, myid, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}