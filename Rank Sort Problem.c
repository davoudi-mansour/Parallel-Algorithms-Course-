
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
/*
MansurDavoudi 1397/08/15 Exercise #2 Rank Sort Problem
method1---type 1---> generate n Random numbers and write them in file,
IMPORTANT-----> first number is in file is n and others are generated numbers

method2---type2---> Performs Rank Sort sequentialy 

method3---type3---> Performs Rank Sort Parallel
To Run type mpirun -n procNumber a.out type filename
*/
int main(int argc, char** argv) {
	printf(" Mansur Davudi 9731568 Exercise #2 Rank Sort 1397/08/15 type1-> generate Random numbers, type2-> perform rank sort sequentialy, type3-> perform Rank sort Parallel \n");
	int type=atoi(argv[1]);
        switch(type){
	case 1:
	  method1(argc,argv);
          break;
	case 2:
	   method2(argc,argv);
	   break;
	case 3:
	   method3(argc,argv);
	   break;
	default :
	   printf(" Enter valid type, 1 or 2 or 3");
	   exit(1);
}
    
    return (EXIT_SUCCESS);
}
void method1(int argc,char **argv){

    int n=atoi(argv[2]);
    int lower=atoi(argv[3]);
    int upper=atoi(argv[4]);
    char *fname=argv[5];
    int result;
    FILE *fp;
    fp=fopen(fname,"w");
       if(fp == NULL)
    {
        printf("error");
        exit(1);
    }
     fprintf (fp,"%d",n);
      fprintf (fp,"\n");
//generate Random Number and write in file
    for(int i=0;i<n;i++)
    {
        result=(rand() % (upper - lower +1)) + lower;
        fprintf(fp,"%4d",result);
               
    }
}
// 
void method2(int argc, char **argv){
   char *fname=argv[2];
    int n;
    FILE *fp;
    fp=fopen(fname,"r");
       if(fp == NULL)
    {
        printf("error");
        exit(1);
    }
    fscanf(fp,"%d",&n);
        int *result=(int*) malloc(n * sizeof(int));
        int *rank=(int*) malloc(n * sizeof(int));
        int *arr=(int*) malloc(n * sizeof(int));
//perform rank sort using rank array
        for(int i=0;i<n;i++)
        {
            rank[i]=0;
        }
          for(int i=0;i<n;i++)
        {
            result[i]=0;
        }
    for(int i=0;i<n;i++)
    {
        fscanf(fp,"%d",&arr[i]);
    }
       for(int i=0;i<n;i++)
       {
           for(int j=0;j<i;j++)
           {
               if(arr[j]>arr[i])
                   rank[j]++;
               else if(arr[i]>=arr[j])
               rank[i]++;
           }
       }
        for(int i=0;i<n;i++)
        {
            result[rank[i]]=arr[i];
        }
        printf("Before Sorting ---> ");
          for(int i=0;i<n;i++)
        {
            printf("%7d",arr[i]);
        }
        printf("\n After Sorting --->");
        for(int i=0;i<n;i++)
        {
            printf("%7d",result[i]);
        }
	printf("\n");
}
void method3(int argc, char **argv){
   
    char *filename=argv[2];
    int myid,procs,err,n,k,startp,endp;
    FILE *fp;
    MPI_Status status;
    err=MPI_Init(&argc , &argv);
    if (err != MPI_SUCCESS) {
	printf("Error in init MPI\n");
	MPI_Abort (MPI_COMM_WORLD, err);
	}// end if

	 // Get No. of processors
MPI_Comm_size(MPI_COMM_WORLD, &procs);

	// Get processor id
MPI_Comm_rank(MPI_COMM_WORLD, &myid);
// receive generated numbers from master and compute partial rank array and send it to mster
if(myid != 0)   
{
     MPI_Recv(&n,1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
     k=n/procs;
     int *arr=(int*) malloc(n * sizeof(int));

int *rank=(int*) malloc(n * sizeof(int));

for(int i=0;i<n;i++)

{

	rank[i]=0;

}

int *result=(int*) malloc(n * sizeof(int));

for(int i=0;i<n;i++)

{

	result[i]=0;

}

int *sorted=(int*)malloc(n * sizeof(int));


     startp=myid * k;
     endp=startp+k;
     MPI_Recv(arr,n,MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    for(int i=startp;i<endp;i++)
    
        for(int j=0;j<i;j++)
        {
            if(arr[j] > arr[i])
            {
                rank[j]++;
            }
            else if(arr[i] >= arr[j])
            {
                rank[i]++;
            }
        }
    MPI_Send(rank,n,MPI_INT,0,myid,MPI_COMM_WORLD);

}
// Master tasks, read numbers from file and send them to other procs, receive the rank arrays and calculate final rank array and show the result
if(myid==0)
{
    
     fp=fopen(filename,"r");
     if(fp==NULL)
      {
	printf("error");
	exit(1);
      }
        fscanf(fp,"%d",&n);
     int *arr=(int*) malloc(n * sizeof(int));

int *rank=(int*) malloc(n * sizeof(int));

for(int i=0;i<n;i++)

{

	rank[i]=0;

}



int *result=(int*) malloc(n * sizeof(int));

for(int i=0;i<n;i++)

{

	result[i]=0;

}

int *sorted=(int*)malloc(n * sizeof(int));


     k=n/procs;
     startp=myid * k;
     endp=startp+k;
	for(int i=1;i<procs;i++)
 	{
	   MPI_Send(&n,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }
	printf("Before Sorting... \n");
	
      for(int i=0;i<n;i++)
	{
	   fscanf(fp,"%d",&arr[i]);
           printf("%4d",arr[i]);
	}
	printf("\n");
     
	for(int i=1;i<procs;i++)
 	{
	   MPI_Send(arr,n,MPI_INT,i,0,MPI_COMM_WORLD);
        }
    
    
      for(int i=startp;i<endp;i++)
    
        for(int j=0;j<i;j++)
        {
            if(arr[j] > arr[i])
            {
                result[j] ++;
            }
            else
            {
                result[i] ++;
            }
        }
    for(int i=1;i<procs;i++)
    {
        MPI_Recv(rank, n, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        for(int j=0;j<n;j++)
        {
            result[j] = result[j] + rank[j];
        }
    }
    
    for(int i=0;i<n;i++)
    {
        sorted[result[i]]=arr[i];
    }
		printf("After Sorting... \n");
     for(int i=0;i<n;i++)
    {
         printf("%4d", sorted[i]);
    }
printf("\n");
    
}
 MPI_Finalize();
}

