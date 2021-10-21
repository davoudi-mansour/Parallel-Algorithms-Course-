//mpicc filename.c
//mpirun -n p ./a.out 
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#define MAXELT          100
#define INFINITY        32760                  
#define SMALLSIZE       10                       
#define STACKSIZE       100                     

int list[MAXELT+1];                              

struct {                                        
        int a,b;
} stack[STACKSIZE];

int top=-1;                                     
int* merge (int *a,int *b, int len1,int len2)
{
	int *mg=(int*) malloc((len1+len2)*sizeof(int));
	int i=0;
	int j=0;
	int k=0;
	while ((i<len1 )&&(j<len2))
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
		while (i<len1)
			{mg[k]=a[i];k++;i++;}
		while(j<len2)
			{mg[k]=b[j];k++;j++;}
	return mg;
}
int main(int argc,char *argv[])                                    
{
		int myid, procs, err, i, j;
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
int n=20;

	if(myid == 0){
		int i=-1,j;
		
        void quicksort(int);
        for(j=0;j<n;j++){
    		list[j]=rand()%100;
    		printf("%d-",list[j]);	
	}
	for(int i = 1; i < procs ; i++){
			MPI_Send(&n, 1, MPI_INT, i, myid, MPI_COMM_WORLD);
			MPI_Send(list, n, MPI_INT, i, myid, MPI_COMM_WORLD);		
		}
	
        
  }
  if (myid!=0)
  {
  	int n;
		MPI_Recv(&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		int start, end, k = n / (procs - 1);
		start = (myid - 1) * k;
		end = start + k;
		 void quicksort(int);
		int *rec_arr = malloc(n * sizeof(int));
		

		MPI_Recv(rec_arr, n, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		 int a;
		 for(int j=start,a=0;j<end;j++,a++){
    		list[a]=rec_arr[j];
    		//printf("%d-",list[j]);	
	}
	
	quicksort(k-1);
	 }
	
	if (myid!=0)
	{
		int k = n / (procs - 1);
		MPI_Send(list, k, MPI_INT, 0, myid, MPI_COMM_WORLD);	
	
	}
	
	if (myid==0)
	{
		
		int *sorted=(int*) malloc (n*sizeof(int));
		int k = n / (procs - 1);
		int len=k;
		sorted=list;
		for (j=1;j<procs;j++)
		{
			MPI_Recv(list, k, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);	
			sorted=merge (sorted, list, n,k);
			len+=k;
		}	
		printf("\n Sorted List is:\n");
        for (j=0;j<n;j++)
                printf("%d-",sorted[j]);
	}

  
        MPI_Finalize();
		return 0;
}

void interchange(int *x,int *y)                 
{
        int temp;

        temp=*x;
        *x=*y;
        *y=temp;
}

void split(int first,int last,int *splitpoint)
{
        int x,i,j,s,g;

        if (list[first]<list[(first+last)/2]) { 
                s=first;
                g=(first+last)/2;
        }
        else {
                g=first;
                s=(first+last)/2;
        }
        if (list[last]<=list[s])
                x=s;
        else if (list[last]<=list[g])
                x=last;
        else
                x=g;
        interchange(&list[x],&list[first]);     
                                                
        x=list[first];
        i=first;                                
        j=last+1;
        while (i<j) {
                do {                             
                        j--;
                } while (list[j]>x);
                do {
                        i++;                    
                } while (list[i]<x);
                interchange(&list[i],&list[j]);  
        }
        interchange(&list[i],&list[j]);          
        interchange(&list[first],&list[j]);     
                                               
        *splitpoint=j;
}

void push(int a,int b)                      
{
        top++;
        stack[top].a=a;
        stack[top].b=b;
}

void pop(int *a,int *b)                          
{
        *a=stack[top].a;
        *b=stack[top].b;
        top--;
}

void insertion_sort(int first,int last)
{
        int i,j,c;

        for (i=first;i<=last;i++) {
                j=list[i];
                c=i;
                while ((list[c-1]>j)&&(c>first)) {
                        list[c]=list[c-1];
                        c--;
                }
                list[c]=j;
        }
}

void quicksort(int n)
{
        int first,last,splitpoint;

        push(0,n);
        while (top!=-1) {
                pop(&first,&last);
                for (;;) {
                        if (last-first>SMALLSIZE) {
                                                 
                                split(first,last,&splitpoint);
                                                
                                if (last-splitpoint<splitpoint-first) {
                                        push(first,splitpoint-1);
                                        first=splitpoint+1;
                                }
                                else {
                                        push(splitpoint+1,last);
                                        last=splitpoint-1;
                                }
                        }
                        else {                   
                                                
                                insertion_sort(first,last);
                                break;
                        }
                }
        }                                       
}
