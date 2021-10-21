
/* 
 * File:   main.c
 * Author: mansur davudi
 * To Run ----> mpirun -n np ./a.out n a b type
 * type1---> odd-even
 *type2---> bitonic
 * Created on December 20, 2018, 9:48 PM
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
void method2(int argc, char** argv)
{
int myid,procs,err,i1,i2;
     int n=atoi(argv[1]);
     int s=2*n;
     int lower=atoi(argv[2]);
     int upper=atoi(argv[3]);
     int result;
     int *array=(int*) malloc(n*sizeof(int));
     int *temp=(int*) malloc(n*sizeof(int));
     int *local=(int*) malloc(2*n*sizeof(int));


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

    int *f=(int*) malloc(n*procs*sizeof(int));
   if(myid==0)
    {
       printf("Mansur davudi 9731568, Exercise 5, Bitonic Sort: ---> \n");
       printf("Before Sorting ---> \n");
       for(int i=0;i<n*procs;i++)
    {
        f[i]=(rand() % (upper - lower +1)) + lower;
        printf("%5d",f[i]);
    }
     printf("\n");
      
     }

     MPI_Scatter(f, n, MPI_INT, array,n, MPI_INT, 0, MPI_COMM_WORLD);

     int *total=(int*) malloc(n*procs*sizeof(int));
  
      sort(array,n);
      
      MPI_Barrier(MPI_COMM_WORLD);

     int d=log2(procs);
     for (int i = 0 ; i < d ; i++) {
        for (int  j = i ; j >= 0 ; j-- ) {
            int partner = myid ^ (1 << j);

              MPI_Send(array, n, MPI_INT, partner, 0, MPI_COMM_WORLD);
              MPI_Recv(temp,n, MPI_INT, partner, 0, MPI_COMM_WORLD, &status);

              i1=0;
              i2=0;
              
               for(int c=0;c<2*n;c++) 
                 {
                      if( i2>=n || (i1<n && array[i1] <= temp[i2]  ))
                     {
                           local[c]=array[i1];
                            i1++;
                      }
                     else{
                               local[c]=temp[i2];
                                i2++;
                              }
                 }
              
            if ( (( myid >> (i+1)) & 1  ) == (  (myid >> j) & 1) )
            {
                for(int q=0;q<n;q++)
                    array[q]=local[q];
            }
                
            else
            {
                int p=0;
                 for(int q=n;q<2*n;q++)
                 {
                      array[p]=local[q];
                      p++;
                 }
                   
            }
               

        }
}
      MPI_Barrier(MPI_COMM_WORLD);

       MPI_Gather(array, n, MPI_INT,total,n , MPI_INT, 0 , MPI_COMM_WORLD);

 
       if(myid==0)
       {

	   printf("After Sorting ---> \n");
           for(int i=0;i<(n*procs);i++)
           {
               printf("%3d ",total[i]);
           }
	    printf("\n");
       }

     MPI_Finalize();
}


void method1(int argc, char** argv)
{
int myid,procs,err,i1,i2;
     int n=atoi(argv[1]);
     int lower=atoi(argv[2]);
     int upper=atoi(argv[3]);
     int result;
     int *array=(int*) malloc(n*sizeof(int));
     int *temp=(int*) malloc(n*sizeof(int));
     int *local=(int*) malloc(2*n*sizeof(int));


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

    int *f=(int*) malloc(n*procs*sizeof(int));
   if(myid==0)
    {
       printf("Mansur davudi 9731568, Exercise 5, Odd-Even Transposition Sort: ---> \n");
       printf("Before Sorting ---> \n");
       for(int i=0;i<n*procs;i++)
    {
        f[i]=(rand() % (upper - lower +1)) + lower;
        printf("%5d",f[i]);
    }
     printf("\n");
      
     }

     MPI_Scatter(f, n, MPI_INT, array,n, MPI_INT, 0, MPI_COMM_WORLD);

     int *total=(int*) malloc(n*procs*sizeof(int));
  
      sort(array,n);
      
      MPI_Barrier(MPI_COMM_WORLD);

     
     for (int i = 0 ; i < n*procs ; i++) {
         if(i%2==0)
         {
             if(myid%2==0 && myid <= procs-2)
             {
                  MPI_Recv(temp,n, MPI_INT, myid+1, 0, MPI_COMM_WORLD, &status);
                  MPI_Send(array, n, MPI_INT, myid+1, 0, MPI_COMM_WORLD);
                    i1=0;
                    i2=0;
               for(int c=0;c<2*n;c++) 
                 {
                      if( i2>=n || (i1<n && array[i1] <= temp[i2]  ))
                     {
                           local[c]=array[i1];
                            i1++;
                      }
                     else{
                               local[c]=temp[i2];
                                i2++;
                              }

                 }
                    for(int q=0;q<n;q++)
                    array[q]=local[q];
                   
             }
             else if(myid % 2 != 0 && myid <= procs-1)
             {
                    MPI_Send(array, n, MPI_INT, myid-1, 0, MPI_COMM_WORLD);
                    MPI_Recv(temp,n, MPI_INT, myid-1, 0, MPI_COMM_WORLD, &status);
                      i1=0;
                      i2=0;
               for(int c=0;c<2*n;c++) 
                 {

                      if( i2>=n || (i1<n && array[i1] <= temp[i2]  ))
                     {
                           local[c]=array[i1];
                            i1++;
                      }
                     else{
                               local[c]=temp[i2];
                                i2++;
                              }
                 }
                  int p=0;
                 for(int q=n;q<2*n;q++)
                 {
                      array[p]=local[q];
                      p++;
                 }
                 
             }
         }
         else if(i%2 != 0)
         {
             if(myid %2 !=0 && myid <= procs-3)
             {
                    MPI_Send(array, n, MPI_INT, myid+1, 0, MPI_COMM_WORLD);
                    MPI_Recv(temp,n, MPI_INT, myid+1, 0, MPI_COMM_WORLD, &status);
                      i1=0;
                    i2=0;
               for(int c=0;c<2*n;c++) 
                 {
                      if( i2>=n || (i1<n && array[i1] <= temp[i2]  ))
                     {
                           local[c]=array[i1];
                            i1++;
                      }
                     else{
                               local[c]=temp[i2];
                                i2++;
                              }
                 }
                    for(int q=0;q<n;q++)
                    array[q]=local[q];
                   
                    
             }
             else if(myid %2 ==0 && myid != 0 && myid<= procs-2)
             {
                   MPI_Recv(temp,n, MPI_INT, myid-1, 0, MPI_COMM_WORLD, &status);
                   MPI_Send(array, n, MPI_INT, myid-1, 0, MPI_COMM_WORLD);
                      i1=0;
                      i2=0;
               for(int c=0;c<2*n;c++) 
                 {
                      if( i2>=n || (i1<n && array[i1] <= temp[i2]  ))
                     {
                           local[c]=array[i1];
                            i1++;
                      }
                     else{
                               local[c]=temp[i2];
                                i2++;
                              }
                 }
                  int p=0;
                 for(int q=n;q<2*n;q++)
                 {
                      array[p]=local[q];
                      p++;
                 }

                 
             }
                 
                 
         }
 
            }
               
      MPI_Barrier(MPI_COMM_WORLD);

       MPI_Gather(array, n, MPI_INT,total,n , MPI_INT, 0 , MPI_COMM_WORLD);
       if(myid==0)
       {
	   printf("After Sorting ---> \n");
           for(int i=0;i<(n*procs);i++)
           {
               printf("%3d ",total[i]);
           }
	    printf("\n");
       }

     MPI_Finalize();

}

int main(int argc, char** argv) {
      
     int check=atoi(argv[4]);
     if(check==1)
       method1(argc,argv);
     else if(check == 2)
       method2(argc,argv);
     
    return (EXIT_SUCCESS);
}

