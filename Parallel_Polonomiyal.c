
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

/*
 * 
 */

void print(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main(int argc, char** argv) {

    int myid, procs, err;
    MPI_Status status;
    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS) {
        printf("init MPI failed\n");
        MPI_Abort(MPI_COMM_WORLD, err);
    }// end if

    // Get No. of processors
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    // Get processor id
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    int *xpart;
    int *a;
    int x = 2;
    int n = 16;
    int size = log2(n);
    int *recv_buf;
    if (myid == 0) {
        xpart = calloc(size, sizeof (int));
        recv_buf = calloc(size, sizeof (int));
        int sum;
        int recvSum = 0;
        a = calloc(n, sizeof (int));
        //        srand(time(NULL));

        for (int i = 0; i < n; i++) {
            a[i] = (rand() % 5) + 1;
//            a[i] =  1;
        }

        xpart[0] = x;
        sum = a[0];
        for (int i = 1; i < size; i++) {
            xpart[i] = xpart[i - 1] * x;
        }
        printf("xpart in root \n");
        print(xpart, size);
        print(a, n);
        MPI_Bcast(xpart, size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(a, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
        print(recv_buf, size);
        int k;
        for (k = 0; k < size; k++) {
            sum += (xpart[k] * recv_buf[k]);
        }


        for (int i = 1; i < procs; i++) 
        {
            MPI_Recv(&recvSum, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("recvSum from %d in root %d \n",i, recvSum);
            sum += recvSum;
        }


        printf("final value is : %d\n", sum);


    }
    if (myid != 0) {
        xpart = calloc(size, sizeof (int));
        recv_buf = calloc(size, sizeof (int));
        MPI_Bcast(xpart, size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(a, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
        int partsum = 0;
        int tempx = xpart[size - 1];
        for (int i = 1; i < procs; i++) {
            if (myid == i) {
                int k;
                for (k = 1; k < myid; k++) {
                    tempx = tempx*xpart[size-1 ];
                }
                for (int j = 0; j < size; j++) {
                    xpart[j] *= tempx;
                }

                for (int i = 0; i < size; i++) {
                    partsum += xpart[i] * recv_buf[i];
                }

                if (myid == procs - 1) {
                    xpart[size - 1] = 0;
                }
                printf("id %d \n", myid);
                print(xpart, size);
                print(recv_buf, size);
                MPI_Send(&partsum, 1, MPI_INT, 0, 50, MPI_COMM_WORLD);
            }
        }

    }


    MPI_Finalize();
    return (EXIT_SUCCESS);
}

