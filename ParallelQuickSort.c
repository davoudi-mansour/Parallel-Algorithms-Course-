/* 
 * File:   main.c
 * Author: mansur davudi 9731568
 * To Run ----> mpirun -n np a.out n a b 
 * Created on December 25, 2018, 9:48 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

void swap(int *ip, int *xp)
{
    int temp=*ip;
    *ip=*xp;
    *xp=temp;
}
void sort(int a[], int n)
{
      int i, j, min;
    for(i=0;i<n-1;i++)
    {
        min =i;
        for(j=i+1;j<n;j++)
            if(a[j]<a[min])
            {
                min=j;
            }
        swap(&a[i],&a[min]);

    }
}

int main(int argc, char** argv) {

     int myid,procs,err;

     int n=atoi(argv[1]);

     int lower=atoi(argv[2]);

     int upper=atoi(argv[3]);

     int *array=(int*) malloc(n*sizeof(int));

     int pivot;

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

  int *final=(int*) malloc(n*procs*sizeof(int));

  int *maq=(int*) malloc(n*procs*procs*sizeof(int));

  int *temp=(int*) malloc(n*sizeof(int));

  int n1=n+1;

  int count=log2(procs);

  int size;

  for(int i=0;i<=count;i++)
      size += pow(2,i);

 int *f=(int*) malloc(n*procs*sizeof(int));

   if(myid==0)
    {
       printf("Before Sorting ---> \n");
       for(int i=0;i<n*procs;i++)
    {
        f[i]=(rand() % (upper - lower +1)) + lower;
        if(i % 10 == 0) printf("\n");
        printf("%5d",f[i]);


        
    }
     printf("\n\n\n");
      
     }
 

   MPI_Scatter(f, n, MPI_INT, array,n, MPI_INT, 0, MPI_COMM_WORLD);

   sort(array,n);


 int **table=(int**) malloc(size*sizeof(int*));
          for (int i=0;i<size;i++)
                 table[i]=(int*) malloc((n1)*sizeof(int));

  for(int i=0;i<size;i++)
      for(int j=0;j<n1;j++)
         table[i][j]=0;


  for(int i=0;i<n1;i++)
   {
       if(i==(n1-1))
         table[0][i]=n;
    else
    {
       table[0][i]=array[i];
       
       //printf("%5d",array[i]);
    }
      
  }


  //printf("\n");

  MPI_Barrier(MPI_COMM_WORLD);

  int row;

  int period;

  for(int i=0;i<log2(procs);i++)
    {

     row=pow(2,i)-1;

     period=pow(2,i);

    for(int j=0;j<period;j++)
    {
        int len=table[row][n1-1];

        int random = rand() % procs;

        if(myid == random)
          
        {
            int t = pow(2,i+1);

            pivot=table[row][(int)n1/t];    
        }

           MPI_Bcast(&pivot, 1, MPI_INT,0, MPI_COMM_WORLD);
 
           MPI_Barrier(MPI_COMM_WORLD);

           int smallSeg=(2*row)+1;

           int bigSeg=(2*row)+2;

           int i1=0;

           int i2=0;

           for(int c=0;c<n;c++)
           {
               
               if(table[row][c] <= pivot )
               {
                    table[smallSeg][i1]=table[row][c];
                    i1++;
                    table[smallSeg][n]++;
               }
               else if(table[row][c] > pivot )
               {
                    table[bigSeg][i2]=table[row][c];
                    i2++;
                    table[bigSeg][n]++;
               }
           }

           row++;

           MPI_Barrier(MPI_COMM_WORLD);
    }
           MPI_Barrier(MPI_COMM_WORLD);
}

    int start=pow(2,log2(procs))-1;

    for(int i=0;i<procs;i++)

  {

    if(i == myid)
     {
        int c=0;
        for(int s=0;s<n;s++)
           {
             // printf("%3d---> %4d",myid,table[start][s]);

              final[c]=table[start][s];

              c++;  
           }
             // printf("\n");

        for(int j=1;j<procs;j++)
           {
              int *temp2=malloc(n*sizeof(int));

 	      MPI_Recv(temp2,n, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

              for(int t=0;t<n;t++)
                {

                    final[c]=temp2[t];
                    c++;
                }

           }

     }

   else
     {
          int *temp1=malloc(n*sizeof(int));

          for(int q=0;q<n;q++)
           {

              temp1[q]=table[start][q];

              //printf("%3d---> %4d",myid,temp1[q]);

           }
              MPI_Send(temp1, n, MPI_INT,i, 0, MPI_COMM_WORLD);  
           
              //printf("\n");

              
     }

   start++;
}
   if(myid==0) printf("\n\n\n");

   MPI_Barrier(MPI_COMM_WORLD);

   int length=n*procs;

   sort(final,length);

   MPI_Barrier(MPI_COMM_WORLD);

 for(int i=0;i<procs;i++)
   {
      if(i == myid)

        {
           printf("id ---> %2d\n",myid);

            for(int j=0;j< n*procs;j++)
               {
                   if(final[j]!= 0){
                      
		     if(j%10==0) printf("\n"); 
 
                     printf(" %5d",final[j]);
		  }
               }
               
             printf("\n");
         
        }
          MPI_Barrier(MPI_COMM_WORLD);
   }      
	MPI_Finalize();

	return (EXIT_SUCCESS);
}

