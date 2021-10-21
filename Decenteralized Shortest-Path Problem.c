
/* 
 * File:   main.c
 * Author: mansur davudi
 *9731568
 * Created on December 9, 2018, 11:16 PM
 * Decenteralized Parallel Implementation of Shortest-Path Problem
 * To Run ----> mpirun -n 6 ./a.out graph.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
# define work 100
# define terminate 200

// here processor i is responsible to serach around node i and find the shortest path from source to node i.

int main(int argc, char** argv) {
      int myid,procs,err;
      int ack=1;
      int size;
      char *file;
      file=argv[1];
      int newdist;
      int tag;
      FILE *fp;
      fp=fopen(file,"r");
      fscanf(fp,"%d",&size);
      int *dist=(int*) malloc(size * sizeof(int));
      for(int i=0;i<size;i++)
     {
        dist[i]=100;
     }
      
    int **w=(int**) malloc(size*sizeof(int*));
	for (int i=0;i<size;i++)
		 w[i]=(int*) malloc(size*sizeof(int));	
    for(int k=0;k<size;k++)
       for(int l=0;l<size;l++)
          fscanf(fp,"%d",&w[k][l]);

     MPI_Status status;
    err=MPI_Init(&argc , &argv);
    if (err != MPI_SUCCESS) {
	printf("Error in init MPI\n");
	MPI_Abort (MPI_COMM_WORLD, err);
	}
        MPI_Comm_size(MPI_COMM_WORLD, &procs);
        MPI_Comm_rank(MPI_COMM_WORLD, &myid);
// at the first master, if node i has value lower than 100, send that value to processor i, and wait for processor's ack response.
       if(myid ==0)
       {
            printf(" * Author: Mansur Davudi 9731568 \n  * Decenteralized Parallel Implementation of Shortest-Path Problem \n ");
       }
        if(myid == 0)
        {
            for(int j=1;j<size;j++)
            {
                if(w[myid][j] < 100)
                {
                      MPI_Send(&w[myid][j], 1, MPI_INT, j, work, MPI_COMM_WORLD);
                      MPI_Recv(&ack,1, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                }
            }
            for(int i=1;i<size;i++)
                       MPI_Send(&ack,1, MPI_INT, i,200, MPI_COMM_WORLD);

        }
// every slave processor calculate the path through it's current path to source and send the result to corresponding slave processor and wait for ack response
        if(myid != 0)
        {
             int des;
             MPI_Recv(&newdist,1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
             des=status.MPI_SOURCE;
             tag=status.MPI_TAG;
             int d;
             while(tag != 200)
             {
                 if(newdist < dist[myid])
                 {
                     dist[myid]=newdist;
                     for(int j=1;j<size;j++)
                     {
                         if(j!=myid && w[myid][j] < 100)
                         {
                              d=dist[myid]+w[myid][j];
                              MPI_Send(&d, 1, MPI_INT, j,100, MPI_COMM_WORLD);
                              MPI_Recv(&ack,1, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                         }
                     }
                 }
                     MPI_Send(&ack, 1, MPI_INT, des, 1, MPI_COMM_WORLD);
                     MPI_Recv(&newdist,1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                     des=status.MPI_SOURCE;
                     tag=status.MPI_TAG;

             }
        printf("number of nodes are %2d \n",size);
        printf("process: %2d ---> shortest path from  source to node %2d is : %3d \n",myid,myid,dist[myid]);

        }
       MPI_Finalize();

    return (EXIT_SUCCESS);
}

