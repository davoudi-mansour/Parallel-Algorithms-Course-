//mpicc filename.c
//mpirun -n p ./a.out n
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

void bubblesort(int arr[], int n) {
	int i, j;
	for (i = 1; i < n; i++)
		for (j = 0; j < n - i; j++)
			if (arr[j] > arr[j + 1]) {
				int tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}// end if
}


int main(int argc, char *argv[]) {

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
	/*
		if(myid==0){
			printf("Homaun Chaab\n"
			"Homework 2\n"
			"1397/08/13\n"
			"Rnak Sort Program In Sequential And Paralell With There \n"
			"Type Of Input-Type 1:Generate Numbers In Specified Range And Store Them In Selected File\n"
			"Type 2:Applay Rank Sort In Sequential Mode\n"
			"Type 3:Applay Rank Sort In Paralell Mode\n");
		}
	*/	int n = atoi(argv[1]);

	if (myid == 0) {
		int *arr = malloc(n * sizeof(int));
		int *arr2 = malloc(n * sizeof(int));
		int *all = malloc(n * sizeof(int));
		srand(time(0));

		for (int i = 0; i < n; i++) {
			arr[i] = rand() % (20 - 1) + 1;
			printf("%d\n", arr[i]);
		}

		for (int i = 1; i < procs; i++) {
			MPI_Send(&n, 1, MPI_INT, i, myid, MPI_COMM_WORLD);
			MPI_Send(arr, n, MPI_INT, i, myid, MPI_COMM_WORLD);
		}
		int w, cnt = 0, q = 0, cnt2 = 0, j = 0;
		for (int i = 1; i < procs; i++) {
			int v = 0;

			MPI_Recv(&w, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			cnt2 += w;
			MPI_Recv(arr2, n, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			int s = status.MPI_TAG;

			//	printf("%d\n",s);
			for (int i = q; i < cnt2; i++) {

				if (arr2[v] != 0) {
					all[i] = arr2[v];
					cnt++;
					v++;
					//			break;
				}

			}

			q = w;
			//	printf("S=%d-Q=%d\n",s,q);
		}
		for (int i = 0; i < n; i++) {
			//	if(all[i]!=0)
			printf("%3d", all[i]);
		}
	}
	else {
		int n;
		MPI_Recv(&n, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		int start, end, k = n / (procs - 1);
		start = (myid - 1) * k;
		end = start + k;
		/*	printf("myid=%d\tk=%d\n",myid,k);
			printf("start=%d\n",start);
			printf("end=%d\n",end);*/

		int *rec_arr = malloc(n * sizeof(int));
		int *send_arr = malloc(n * sizeof(int));
		int *bucket = malloc(n * 10 * sizeof(int));
		int *sorted = malloc((n * 10) * sizeof(int));

		MPI_Recv(rec_arr, n, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);


		int div = (20 + 1) / n, c;
		//printf("div=%d\n",div);
/*    	for (i = 0; i < n; i++)
				bucket[i] = 0;
	*/    		for (int i = 0; i < n; i++) {
		for (int j = 0; j < 10; j++) {
			bucket[i * 10 + j] = 0;
			//printf("bucket[%d]\t%d\n",i+j,bucket[i+j]);
		}
	}
		int j = 0;
		for (int i = 0; i < n; i++) {
			c = rec_arr[i] / div;
			if (start <= c && c <= end) {
				if (bucket[c * 10 + j] == 0) {
					bucket[c * 10 + j] = rec_arr[i];

				}
				else
				{
					j++;
					bucket[c * 10 + j] = rec_arr[i];

				}
			}
		}

		int cnt = 0;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 10; j++) {
				//	if(bucket[i*5+j]!= 0)
				send_arr[j] = bucket[i * 10 + j];
			}
			bubblesort(send_arr, 10);
			for (int j = 0; j < 10; j++) {

				if (send_arr[j] != 0) {
					sorted[cnt] = send_arr[j];
					cnt++;
				}



			}
			//	printf("%5d",sorted[i]);

		}


		int w = 0;
		for (int i = 0; i < n; i++) {
			if (sorted[i] != 0) {
				//	printf("%d-",sorted[i]);
				w++;
			}
		}

		//	printf("myid=%d-W=%d\n",myid,w);
		MPI_Send(&w, 1, MPI_INT, 0, myid, MPI_COMM_WORLD);
		MPI_Send(sorted, n, MPI_INT, 0, myid, MPI_COMM_WORLD);




	}
	MPI_Finalize();
	return 0;
}
