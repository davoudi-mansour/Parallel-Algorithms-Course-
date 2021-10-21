
/* 
 * File:   main.c
 * Author: mansur davudi
 * 9731568
 * Created on December 6, 2018, 3:49 AM
 * Centeralized Parallel Implementation of Shortest-Path Problem
 * To Run ----> mpirun -n 4 ./a.out graph.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
# define work 100
# define terminate 200
# define endTask 300
# define Task 400

int size;
int front=-1 ; int rear=-1;

void enqueue(int *item_queue, int value){
    if(rear != size-1)
    {
         if(front == -1)
            front = 0;
        rear++;
        item_queue[rear] = value;
    }
   
}
int isEmpty(){
    int check=1;
    if(front== -1)
         check=0;
    
    return check;
}
int dequeue(int *item_queue){
    int value=0;
    if(front != -1)
    {
        value=item_queue[front];
        front++;
          if(front > rear)
            front = rear = -1;
        return value;
    }
   
}

int main(int argc, char** argv) {
      int myid,procs,err;
      int request;
      char *file;
      file=argv[1];

      FILE *fp;
      fp=fopen(file,"r");
      fscanf(fp,"%d",&size);
    int *item_queue=(int*) malloc(size * sizeof(int));
    int *dist=(int*) malloc(size * sizeof(int));
    int **w=(int**) malloc(size*sizeof(int*));
	for (int i=0;i<size;i++)
		 w[i]=(int*) malloc(size*sizeof(int));
	
    for(int k=0;k<size;k++)
       for(int l=0;l<size;l++)
          fscanf(fp,"%d",&w[k][l]);

    for(int i=0;i<size;i++){
      if(i!=0)
        dist[i]=w[0][i];
      else{
        dist[i]=0;
       }

    }
     MPI_Status status;
    err=MPI_Init(&argc , &argv);
    if (err != MPI_SUCCESS) {
	printf("Error in init MPI\n");
	MPI_Abort (MPI_COMM_WORLD, err);
	}
        MPI_Comm_size(MPI_COMM_WORLD, &procs);
        MPI_Comm_rank(MPI_COMM_WORLD, &myid);
        if(myid ==0)
       {
            printf(" * Author: Mansur Davudi 9731568 \n  * Centeralized Parallel Implementation of Shortest-Path Problem \n ");
       }
        if(myid == 0)
        {
            int j,path,des,tag;
            int v;
// nodes that have no direct path to source have distance 100, at the first
// master put nodes that have direct path to source in work pool(item_queue)

         for(int i=0;i<size;i++)
             if(dist[i] < 100 && dist[i]>0)
                 enqueue(item_queue , i);
// in an infinite while master receive request for work and send nodes available in work pool to slaves

         while(1 == 1)
         {
               if(isEmpty() == 0)
                   break;
               MPI_Recv(&request,1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
               des=status.MPI_SOURCE;
               v=dequeue(item_queue);
               MPI_Send(&v, 1, MPI_INT, des, work, MPI_COMM_WORLD);
               MPI_Send(dist, size, MPI_INT, des, work, MPI_COMM_WORLD);
               MPI_Recv(&j,1, MPI_INT, des, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
               MPI_Recv(&path,1, MPI_INT,des, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
               tag=status.MPI_TAG;
// while tag != endTask master receive result nodes from slaves and put nodes to work pool
               while(tag != endTask)
               {
                   enqueue(item_queue,j);
                   dist[j]=path;
                   MPI_Recv(&j,1, MPI_INT, des, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                   MPI_Recv(&path,1, MPI_INT, des, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                   tag=status.MPI_TAG; 
               }
               
         }
// here master send terminate tag to slaves to terminate them

           for(int i=0;i<procs-1;i++)
           {
                 MPI_Recv(&request,1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                 des=status.MPI_SOURCE;
                 MPI_Send(&v, 1, MPI_INT, des, terminate, MPI_COMM_WORLD);
                 MPI_Send(dist, size, MPI_INT, des, terminate, MPI_COMM_WORLD);
           }
           printf("number of nodes are %2d \n",size);
           for(int i=0;i<size;i++)
           {
               printf(" shortest path from node 0 to : %2d is ---> %3d\n",i,dist[i] );
           }
        }
        if(myid != 0)
        {
            int v,tag,new_dist;
            int request=0;
            MPI_Send(&request, 1, MPI_INT, 0, myid, MPI_COMM_WORLD);
            MPI_Recv(&v,1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(dist,size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            tag=status.MPI_TAG;
// slave process receive one node from master, and find nodes that have shorter path to source through that source and return them to master

            while(tag == work)
            {
                for(int i=0;i<size;i++)
                {
                    if(w[v][i] < 100)
                    {
                          new_dist=dist[v] + w[v][i];
                          if(new_dist < dist[i])
                          {
                                 dist[i]=new_dist;
                                 MPI_Send(&i, 1, MPI_INT, 0,Task, MPI_COMM_WORLD);
                                 MPI_Send(&new_dist, 1, MPI_INT, 0, Task, MPI_COMM_WORLD);
                          }

                    }
                   
                }
		int r=0;
                MPI_Send(&r, 1, MPI_INT, 0,endTask, MPI_COMM_WORLD);
                MPI_Send(&r, 1, MPI_INT, 0, endTask, MPI_COMM_WORLD);
                MPI_Send(&request, 1, MPI_INT, 0, myid, MPI_COMM_WORLD);
                MPI_Recv(&v,1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                MPI_Recv(dist,size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                tag=status.MPI_TAG;
            }

        }

    MPI_Finalize();
    return (EXIT_SUCCESS);
}

