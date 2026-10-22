#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define SIZE 500000
 
int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // The current process ID / Rank.
 
    MPI_Request requests[2];
    int flag;
    int num1[SIZE], num2[SIZE];
 
    if (rank == 0) {
        for (int i = 0; i < SIZE; i++) {
            num1[i] = 100;
        }
        
        MPI_Isend(&num1, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&num2, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &requests[1]);
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    } else {
        for (int i = 0; i < SIZE; i++) {
            num2[i] = 200;
        }

        MPI_Isend(&num2, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(&num1, SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &requests[1]);
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    }

    MPI_Finalize();
 
}
