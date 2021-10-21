//mpicc filename.c -lm
//mpirun -n p ./a.out 
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>


int main(int argc, char *argv[]) {
	srand(time(NULL));
	int myid, procs, err;
	MPI_Status status;
	err = MPI_Init(&argc, &argv);
	if (err != MPI_SUCCESS) {
		printf("Error in init MPI\n");
		MPI_Abort(MPI_COMM_WORLD, err);
	}// end if
	MPI_Comm_size(MPI_COMM_WORLD, &procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	int n = 16, i, j, x, y;
	//int min = 0;
	//int max = 9;
	//int range = max - min + 1;
	int range = n;
	int *a = malloc((n) * sizeof(int));
	int *c = malloc((range) * sizeof(int));
	int *total = (int*)malloc(range * sizeof(int));

	if (myid == 0) {
		int *count = malloc((range) * sizeof(int));
		for (i = 0; i < n; i++) {
			int scale = rand() % (range);
			a[i] = scale;
		}
		for (i = 0; i < n; i++) {
			printf("%d\t", a[i]);
		}
		for (i = 0; i < range; i++) {
			c[i] = 0;
			count[i] = 0;
		}
		printf("\n");
		for (int i = 1; i < procs; i++) {
			//MPI_Send(&n, 1, MPI_INT, i, myid, MPI_COMM_WORLD);
			MPI_Send(a, n, MPI_INT, i, myid, MPI_COMM_WORLD);
			MPI_Send(c, range, MPI_INT, i, myid, MPI_COMM_WORLD);
		}

		for (int j = 1; j < procs; j++) {
			MPI_Recv(count, range, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			for (i = 0; i < range; i++)
				c[i] += count[i];
		}
		printf("\n");
		for (i = 0; i < range; i++) {
			printf("%d\t", c[i]);
		}

		for (j = 1; j < procs; j++) {
			MPI_Send(c + j - 1, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
		}
	}

	else {
		//MPI_Recv(&n, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		int start, end, k = n / (procs - 1);
		start = (myid - 1) * k;
		end = start + k;
		MPI_Recv(a, n, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(c, range, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		for (i = start; i < end; i++) {
			c[a[i]]++;
		}
		MPI_Send(c, range, MPI_INT, 0, myid, MPI_COMM_WORLD);

		MPI_Recv(&y, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		//printf("\nmyid = %d\n", myid);
		for (i = 0; i <= log(n)+1; i++) {
			if (myid <= n - pow(2, i)){
				//printf("*myid = %d", myid);
				MPI_Send(&y, 1, MPI_INT, myid + pow(2, i), 0, MPI_COMM_WORLD);

			}
			if (myid > pow(2, i)) {
				MPI_Recv(&x, 1, MPI_INT, myid - pow(2, i), MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				y += x;
			}
		}
		MPI_Send(&y, 1, MPI_INT, 0, myid, MPI_COMM_WORLD);
	}

	//if (myid != 0)
	//	MPI_Send(&y, 1, MPI_INT, 0, myid, MPI_COMM_WORLD);

	if (myid == 0) {
		//total[0] = y;
		for (j = 1; j < procs; j++) {
			MPI_Recv(&x, 1, MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			total[j-1] = x;
		}
		printf("\n");
		//for (j = 0; j < range; j++) {
		//	printf("%d\t", total[j]);
		//}

		int *b = malloc((range) * sizeof(int));
		
		//for (j = 0; j < range; j++) {
		//	printf("%d\t", total[j]);
		//}

		printf("\n");

		for (j = 0; j < range; j++) {
			b[total[a[j]]] = a[j];
			total[a[j]]--;
		}
		
		for (j = 1; j <= range; j++) {
			printf("%d\t", b[j]);
		}
	}

	MPI_Finalize();
}


