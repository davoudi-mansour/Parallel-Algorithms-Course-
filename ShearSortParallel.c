//mpicc filename.c -std=c99 -lm
//mpirun -n p ./a.out numbers.txt
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
#include <sys/time.h>
#include "mpi.h"


void matrix_decompose(int *,int,int,int *,int *);
void sheer_sort_function(int *,int,int);
bool odd_even_function(int * ,int);
void merge_2D_function(int *,int,int);

int main(int argc, char *argv[])
{
	//int array[24]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
	//int array[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

	//READ FROM FILE AND GET THE COUNT. WILL CREfATE ARRAY OF THAT SIZE AND LOAD INTO IT.
	int count;
	int numProcessors;
	int i=0;
	int c;
	int temp;
	//int * array;
	FILE* f = fopen("numbers.txt","r");
	fscanf (f,"%d",&count);
		
	//array  = (int *) malloc(count*sizeof(int));
	int array[20000];
	
	while(i!=count)
	{
		fscanf(f,"%d", &temp);
		array[i]=temp;
        i++;
	}
	
	fclose(f);
	printf("\n");
	
	time_t rawtime;
	struct tm * timeinfo;
	struct timeval start_time, end_time;
	
	// MPI Initialization
	int    size;
	int    my_rank;
	MPI_Status  status;
	MPI_Comm    comm;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	numProcessors = size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int buff[1];
	buff[0] = 99;
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
	
	printf("\nNo of processes - %d , My rank - %d",size,my_rank);
	int row;
	int col;
	if (my_rank == 0)
	{
		printf("\n MASTER NODE No of processes - %d , My rank - %d\n",size,my_rank);
		int numSlaves = numProcessors - 1;
		// Start the timer here.. Matrix decomposition process
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		printf ( "Current local time and date: %s", asctime (timeinfo) );
		gettimeofday(&start_time, NULL);
		printf("%ld\n", ((start_time.tv_sec * 1000000 + start_time.tv_usec)));
		//Matrix decomposition
		matrix_decompose(array,count,numSlaves,&row,&col);
		int sendbuff[20000];
		int row_col_info[2];
		
		int start =0;
		int end = row*col;	
		row_col_info[0]=row;
		row_col_info[1]=col;
				
		for(int i=1;i<numProcessors;i++)
		{
			//Now every processor will get row*col elements.
			//printf("\n Processor %d array ",i);
			
			for(int j=start,k=0;j<end;j++,k++)
			{
				//printf("%d ",*(array+j));
				sendbuff[k] = *(array+j);
			}
			start += (row*col);
			end += (row*col);
			//printf("\nmessage to be sent-");
			//for(int k=0;k<(row*col);k++)
			//{
			//	printf("%d ",sendbuff[k]);
			//}
			
			//Sending row,col information to slaves. //TAG 0
			printf("\nMaster Sending messages to %d ",i);
			MPI_Send(row_col_info,2,MPI_INT,i,0,comm);	
			//Send actual message 
			//printf("\nSending message to %d ",i);	//TAG 1
			MPI_Send(sendbuff,row*col,MPI_INT,i,1,comm);
		}
		
		//Wait till receives messages from all slaves
		int *slave_array;
		slave_array = (int *) malloc(row*col*sizeof(int));
		
		int matrix[numSlaves][row*col];
		
		int *merge_array;
		int merge_array_length = row*col*numSlaves;
		merge_array = (int *) malloc(merge_array_length*sizeof(int));
		int start_mem_add = 0;
		for(int i=0;i<numSlaves;i++)
		{
			//printf("\nMASTER MASTER MASTER waiting MASTER MASTER MASTER %d\n ",start_mem_add);
			MPI_Recv(slave_array, row*col, MPI_INT, MPI_ANY_SOURCE, 2, comm,&status);
			printf("\n@MASTER @MASTER @MASTER slave input received @MASTER @MASTER @MASTER\n");
			for(int j=0;j<(row*col);j++)
			{
				matrix[i][j] = slave_array[j];
			}
			start_mem_add += (row*col);
		}
		printf("\n");
		//for(int i=0;i<numSlaves;i++)
		//{
		//	for(int j=0;j<(row*col);j++)
		//	{
		//		printf("matrix_array %d ",matrix[i][j]);
		//	}
		//}
		int index=0;
		int start_value = 0; int end_value = col;
		for(int i=0;i<row;i++)
		{
			for(int j=0;j<numSlaves;j++)
			{
				for(int k=start_value;k<end_value;k++)
				{
					//printf(" %d ",matrix[j][k]);
					merge_array[index]=matrix[j][k];
					index = index + 1;
				}
			}
			start_value = start_value + col;
			end_value = end_value + col;
		}
		printf("\n merge_array \n");
		for(int i=0;i<merge_array_length;i++)
		{
			printf(" %d ",merge_array[i]);
		}
		//******************************** 2D merge **********************************/
		merge_2D_function(merge_array,(merge_array_length/(row*col)),(row*col));
		//Calculate end time
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		printf ( "Current local time and date: %s", asctime (timeinfo) );
		gettimeofday(&end_time, NULL);
		printf(" End time %ld\n", ((end_time.tv_sec * 1000000 + end_time.tv_usec)));
		printf("######### Total time in micro seconds  %ld\n", ((end_time.tv_sec * 1000000 + end_time.tv_usec)
							- (start_time.tv_sec * 1000000 + start_time.tv_usec)));
		//******************************** DISPLAY FINAL SORTED OUTPUT **********************************/
		printf("******************************** DISPLAY FINAL SORTED OUTPUT **********************************");
		printf("\n\n\n\n");
		printf("Snake indexed output - rows=%d columns=%d\n",(merge_array_length/(row*col)),(row*col));
		for(int i=0;i<merge_array_length;i++)
		{
			printf(" %d ",merge_array[i]);
		}
		printf("\n**********************Sorted list****************************\n");
		int output_matrix[merge_array_length/(row*col)][(row*col)];

		int begin=0;
		for(int i=0;i<(merge_array_length/(row*col));i++)
		{
			for(int j=0,k=begin;j<(row*col);j++,k++)
			{
				output_matrix[i][j] = merge_array[k];
			}
			begin = begin+(row*col);
		}
		/*for(int i=0;i<row;i++)
		{
			for(int j=0;j<col;j++)
			{
				printf(" %d ",output_matrix[i][j]);
			}
			printf("\n");
		}*/
		//
		for(int i=0;i<(merge_array_length/(row*col));i++)
		{
			if(i%2==0)
			{
				for(int j=0;j<(row*col);j++)
				{
					if(output_matrix[i][j] != -1)
					{
						printf(" %d ",output_matrix[i][j]);
					}
				}
			}
			else
			{
				for(int j=(row*col)-1;j>=0;j--)
				{
					if(output_matrix[i][j] != -1)
					{
						printf(" %d ",output_matrix[i][j]);
					}	
				}
			}
			//printf("\n");
		}
	}
	else
	{
		//Slave nodes
		printf("\nI am slave node \n");
		int row_col_info[2];
		MPI_Recv(row_col_info, 2, MPI_INT, 0, 0, comm,&status);
		row = row_col_info[0];
		col = row_col_info[1];
		
		int rcvbuff[20000];
		MPI_Recv(rcvbuff, count, MPI_INT, 0, 1, comm,&status);
		int rcv_count;
		MPI_Get_count(&status, MPI_INT, &rcv_count);
		//printf("\n Rank %d received %d values & first is %d = row %d,col %d ",my_rank,rcv_count,rcvbuff[0],row,col);
		//Now slave node received evrything : submatrix,row,col . So can start calling shear sort.
		//**********  Shear sort processing ********************/
		sheer_sort_function(rcvbuff,row,col);
		
		printf("\nSENDING MESSAGE BACK TO MASTER\n");	//TAG 2
		MPI_Send(rcvbuff,rcv_count,MPI_INT,0,2,comm);
		
	}
	//getchar();
	MPI_Finalize();
	return 0;
}

void matrix_decompose(int *ptr,int length,int numProcessors,int *row,int * col)
{
	// It will be used by master node. Decompose the tasks for all slaves.
	
	int append_count = 0;
	int count = length%numProcessors;
	int paddingCount = numProcessors - count;
	
	printf("%d \n",paddingCount);
	if(count !=0)
	{
		int oldLength = length;
		length = length+paddingCount; //Padding logic
		for(int i=oldLength;i<length;i++)
		{
			*(ptr+i) = -1;
			append_count +=1;
		}
	}
	printf("New lngth after padding %d \n",length);
	//for(int i=0;i<length;i++)
	//{
	//	printf("%d ",*(ptr+i));
	//}
	
	//Check whether number of elements is perfect square.
	bool perfect_square = false;
	double d_length = length/numProcessors;
	double d_sqrt = sqrt(d_length);
	int i_sqrt = d_sqrt;
	if(i_sqrt == d_sqrt)
	{
		//printf("\nperfect square");
		perfect_square = true;
	}
	else
	{
		//printf("\nNOT perfect square");
	}
	//Determine rows and columns in sub matrices
	if(perfect_square)
	{
		*row = i_sqrt;
		*col = i_sqrt;
	}
	else
	{
		int submatrix_count = length/numProcessors;
		printf("\n N in each sub matrices %d\n",submatrix_count);
		if(submatrix_count%2 != 0)
		{
			submatrix_count +=1;
			int oldLength = length;
			length = length+(numProcessors);
			for(int i=oldLength;i<length;i++)
			{
				*(ptr+i) = -1;
				append_count +=1;
			}
		}
		if(submatrix_count==2)
		{
			*row = 2;
			*col = 1;
		}
		else
		{
			*row = 2;
			*col = submatrix_count/2;
		}
		if(submatrix_count==1)
		{
			*row = 1;
			*col = 1;
		}
	}
	//Debugging
	//printf("\n new lngth %d ",length);
	//for(int i=0;i<length;i++)
	//{
	//	printf("%d ",*(ptr+i));
	//}
	printf("\nrow %d ",*row);
	printf("\ncol %d ",*col);
	//Debugging end

	/*YOU CAN ADD LOAD BALANCING CODE HERE*/
}

	
/* ******************************SHEER SORT FUNCTIONS*******************************************/	
void sheer_sort_function(int *ptr,int nRows,int nColumns)
	//nRows is number of rows and nColumns is number of columns
{
	bool sorted = false;
	bool endFlag = false;
	while(!endFlag)
	{
		for(int i=0;i<nRows;i+=2){
			//Even rows
			int * evenRowArray;
			//evenRowArray = new int[nColumns];
			evenRowArray = (int*) malloc(nColumns*sizeof(int));
			for(int j=0;j<nColumns;j++){
				//Build an array which needs to be odd-even sorted.
				//printf("%d ",*(ptr+(i*nColumns)+j));
				evenRowArray[j] = *(ptr+(i*nColumns)+j);
			}
			////////
			/** USE ODD_EVEN SORT FUNCTION***/
			endFlag = odd_even_function(evenRowArray,nColumns);
			////////
			//debugging
			//printf("\n Even Row sorted output is - ");
			//for(int i=0;i<nColumns;i++)
			//{
			//	printf("%d ",evenRowArray[i]);
			//}
			//debugging end
			for(int j=0;j<nColumns;j++){
				//Put values back in original array
				*(ptr+(i*nColumns)+j) = evenRowArray[j];
			}
		}
		//
		for(int i=1;i<nRows;i+=2){
			//Odd rows
			int * oddRowArray;
			//oddRowArray = new int[nColumns];
			oddRowArray = (int*) malloc(nColumns*sizeof(int));
			for(int j=0;j<nColumns;j++){
				//Build an array which needs to be odd-even sorted.
				//printf("%d ",*(ptr+(i*nColumns)+j));
				oddRowArray[j] = *(ptr+(i*nColumns)+j);
			}
			////////
			/** USE ODD_EVEN SORT FUNCTION***/
			endFlag = odd_even_function(oddRowArray,nColumns);
			////////
			//Reverse array
			for(int i=0,j=nColumns-1;i<nColumns/2;i++,j--)
			{
				int tmp = oddRowArray[i];
				oddRowArray[i] = oddRowArray[j];
				oddRowArray[j] = tmp;
			}
			//debugging
			//printf("\n Odd Row sorted output is - ");
			//for(int j=0;j<nColumns;j++)
			//{
			//	printf("%d ",oddRowArray[j]);
			//}
			//debugging end
			for(int j=0;j<nColumns;j++)
			{
				//Put values back in original array
				*(ptr+(i*nColumns)+j) = oddRowArray[j];
			}
		}
		//
		for(int i=0;i<nColumns;i++)
		{
			//Column sorting
			//printf("Column %d is ",i);
			int * columnArray;
			//columnArray = new int[nRows];
			columnArray = (int*) malloc(nRows*sizeof(int));
			for(int j=0;j<nRows;j++)
			{
				//printf("%d ",*(ptr+i+(j*nRows)));
				//columnArray[j]= *(ptr+i+(j*nRows));
				//printf("%d",*(ptr+i+(j*nColumns)));
				columnArray[j]= *(ptr+i+(j*nColumns));
			}
			////////
			/** USE ODD_EVEN SORT FUNCTION***/
			endFlag = odd_even_function(columnArray,nRows);
			////////
			//debugging
			//printf("\n Column sorted output is - ");
			//for(int i=0;i<nRows;i++)
			//{
			//	printf("%d ",columnArray[i]);
			//}
			//debugging end
			for(int j=0;j<nRows;j++)
			{
				//Put values back in original column
				*(ptr+i+(j*nColumns)) = columnArray[j];
			}
		}

	}
	
}

bool odd_even_function(int *ptr,int length)
{
	/***** Just pass on the array pointer whih needs to sort *******/
	//printf("in function\n");
	//printf("%d %d\n",*ptr,length);
	for(int i=0;i<length;i++)
	{
		//printf("%d %d\n",*(ptr+i),length);
	}
	bool sorted = false;
	bool endFlag = true;	//End of shear sort

	while(!sorted)
	{
		sorted=true;
		for(int i = 1; i < length-1; i += 2)
		{
			if(*(ptr+i) > *(ptr+i+1))
			{
				//printf("Odd sort\n");
				int tmp = *(ptr+i);
				*(ptr+i) = *(ptr+i+1);
				*(ptr+i+1) = tmp;
				sorted = false;
				//
				endFlag = false;
			}
		}
 
		for(int i = 0; i < length-1; i += 2)
		{
			if(*(ptr+i) > *(ptr+i+1))
			{
				//printf("Even sort\n");
				int tmp = *(ptr+i);
				*(ptr+i) = *(ptr+i+1);
				*(ptr+i+1) = tmp;
				sorted = false;
				//
				endFlag = false;
			}
		}
	}
	return endFlag;
}

/******************  2D merge processing  **************************/
void merge_2D_function(int *ptr,int nRows,int nColumns)
{
	// Column sorting
	// Two iterations of shear sort.
	printf("\n############### 2D MERGE PROCESS STARTED ##########\n");
	bool endFlag=false; //Useless
	for(int i=0;i<nColumns;i++)
	{
		//Column sorting
		//printf("Column %d is ",i);
		int * columnArray;
		//columnArray = new int[nRows];
		columnArray = (int*) malloc(nRows*sizeof(int));
		for(int j=0;j<nRows;j++)
		{
			//printf("%d ",*(ptr+i+(j*nRows)));
			//columnArray[j]= *(ptr+i+(j*nRows));
			//printf("%d",*(ptr+i+(j*nColumns)));
			columnArray[j]= *(ptr+i+(j*nColumns));
		}
		////////
		/** USE ODD_EVEN SORT FUNCTION***/
		endFlag = odd_even_function(columnArray,nRows);
		////////
		//debugging
		//printf("\n Column sorted output is - ");
		//for(int i=0;i<nRows;i++)
		//{
		//	printf("%d ",columnArray[i]);
		//}
		//debugging end
		for(int j=0;j<nRows;j++)
		{
			//Put values back in original column
			*(ptr+i+(j*nColumns)) = columnArray[j];
		}
		
		//delete [] columnArray;
		//printf("\nArray deleted\n");
	}
	//2 iterations of shear sort
	
	for(int x=0;x<2;x++)
	{
		for(int i=0;i<nRows;i+=2){
			//Even rows
			int * evenRowArray;
			//evenRowArray = new int[nColumns];
			evenRowArray = (int*) malloc(nColumns*sizeof(int));
			for(int j=0;j<nColumns;j++){
				//Build an array which needs to be odd-even sorted.
				//printf("%d ",*(ptr+(i*nColumns)+j));
				evenRowArray[j] = *(ptr+(i*nColumns)+j);
			}
			////////
			/** USE ODD_EVEN SORT FUNCTION***/
			endFlag = odd_even_function(evenRowArray,nColumns);
			////////
			//debugging
			//printf("\n Even Row sorted output is - ");
			//for(int i=0;i<nColumns;i++)
			//{
			//	printf("%d ",evenRowArray[i]);
			//}
			//debugging end
			for(int j=0;j<nColumns;j++){
				//Put values back in original array
				*(ptr+(i*nColumns)+j) = evenRowArray[j];
			}
			//delete [] evenRowArray;
			//printf("\nArray deleted\n");
		}
		//
		for(int i=1;i<nRows;i+=2){
			//Odd rows
			int * oddRowArray;
			//oddRowArray = new int[nColumns];
			oddRowArray = (int*) malloc(nColumns*sizeof(int));
			for(int j=0;j<nColumns;j++){
				//Build an array which needs to be odd-even sorted.
				//printf("%d ",*(ptr+(i*nColumns)+j));
				oddRowArray[j] = *(ptr+(i*nColumns)+j);
			}
			////////
			/** USE ODD_EVEN SORT FUNCTION***/
			endFlag = odd_even_function(oddRowArray,nColumns);
			////////
			//Reverse array
			for(int i=0,j=nColumns-1;i<nColumns/2;i++,j--)
			{
				int tmp = oddRowArray[i];
				oddRowArray[i] = oddRowArray[j];
				oddRowArray[j] = tmp;
			}
			//debugging
			//printf("\n Odd Row sorted output is - ");
			//for(int j=0;j<nColumns;j++)
			//{
			//	printf("%d ",oddRowArray[j]);
			//}
			//debugging end
			for(int j=0;j<nColumns;j++)
			{
				//Put values back in original array
				*(ptr+(i*nColumns)+j) = oddRowArray[j];
			}

			//delete [] oddRowArray;
			//printf("\nArray deleted\n");
		}
		//
		for(int i=0;i<nColumns;i++)
		{
			//Column sorting
			//printf("Column %d is ",i);
			int * columnArray;
			//columnArray = new int[nRows];
			columnArray = (int*) malloc(nRows*sizeof(int));
			for(int j=0;j<nRows;j++)
			{
				//printf("%d ",*(ptr+i+(j*nRows)));
				//columnArray[j]= *(ptr+i+(j*nRows));
				//printf("%d",*(ptr+i+(j*nColumns)));
				columnArray[j]= *(ptr+i+(j*nColumns));
			}
			////////
			/** USE ODD_EVEN SORT FUNCTION***/
			endFlag = odd_even_function(columnArray,nRows);
			////////
			//debugging
			//printf("\n Column sorted output is - ");
			//for(int i=0;i<nRows;i++)
			//{
			//	printf("%d ",columnArray[i]);
			//}
			//debugging end
			for(int j=0;j<nRows;j++)
			{
				//Put values back in original column
				*(ptr+i+(j*nColumns)) = columnArray[j];
			}
		}

	}
		
}



