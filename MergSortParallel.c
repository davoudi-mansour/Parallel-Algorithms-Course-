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


int main(int argc, char *argv[]) {
srand(time(NULL));
	int myid, procs, err, i, j, type;
	MPI_Status status;

	// initialize MPI_Init
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}// end if

	 // Get No. of processors
	MPI_Comm_size(MPI_COMM_WORLD, &procs);

	// Get processor id
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	int n=8;
	int *array=(int*) malloc(n*sizeof(int));
	int *sarray=(int*) malloc(n*sizeof(int));
	int *sorted=(int*) malloc(n*sizeof(int));
	
	if (myid==0)
	{

		for(int i=0;i<n;i++)
		array[i] = rand()%100;
		for (int i=0;i<n;i++)
			printf("%3d",array[i]);

		for (int i=0;i<log(n)+1;i++)
		{

			for(j=1;j<=(procs-1)/pow(2,i);j++)
			{
//printf(" j is %d  ", j);

				MPI_Send(array, n, MPI_INT, j, 0, MPI_COMM_WORLD);
				MPI_Send(&i, 1, MPI_INT, j, 0, MPI_COMM_WORLD);


			}
int finish=-1;
			for(j=(procs-1)/pow(2,i)+1;j<procs-1;j++)
			{
				MPI_Send(array, n, MPI_INT, j, 0, MPI_COMM_WORLD);
				MPI_Send(&finish, 1, MPI_INT, j, 0, MPI_COMM_WORLD);


			}
			for(j=1;j<=(procs-1)/pow(2,i);j++)
			{
				MPI_Recv(sarray, n, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				int start=(j-1)*pow(2,i+1);
				int end=start+pow(2,i+1);

				for (int k=start;k<end;k++)
					array[k]=sarray[k];

printf ("\n i:%d j:%d  start:%d end:%d:", i, j,  start, end);
				for (int l=0;l<n;l++)
					printf("%4d",array[l]);
				printf("\n");


			}
		

		}
		//for(j=0;j<n;j++)
			//printf("%4d", sorted[j]);
	}
	if (myid!=0)
	{
int flag=true;
while (flag)
{

int count;
		MPI_Recv(sarray, n, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);	
		MPI_Recv(&count, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);	
if (count==-1)
{
flag=false;break;
}
		int M=myid-1;
		//int count=status.MPI_TAG;
		int range=pow(2,count+1);

		int start=M*range;
		int end= start+range;
//printf ("\n %d %d %d %d:", myid, count, start, end);
		int mid= (start+end)/2;
		int *temp=(int*) malloc(range*sizeof(int));
		//int length=range/2;
		int k=0;
		i=start; j= mid;
		while ((i<mid )&&(j<end))
		{
			if (sarray[i]<sarray[j])
				{
					temp[k]=sarray[i];i++;k++;
				}
			if (sarray[i]>=sarray[j])
			{
				temp[k]=sarray[j];k++;j++;
			}
		}

		while (i<mid)
			{
					temp[k]=sarray[i];i++;k++;
				}
		while(j<end)
			{
				temp[k]=sarray[j];k++;j++;
			}
		int h=0;
		for(j=start, h=0;j<end;j++,h++)
			sarray[j]=temp[h];
		MPI_Send(sarray, n, MPI_INT, 0, 0, MPI_COMM_WORLD);
		//for (int i=0;i<n;i++)
				//printf("%4d",sarray[i]);
			//printf("\n");	
	}
	}
MPI_Finalize();
}
