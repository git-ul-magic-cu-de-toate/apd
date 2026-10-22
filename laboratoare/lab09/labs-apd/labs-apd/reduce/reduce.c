#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0

int main (int argc, char *argv[])
{
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value = rank;

    for (int i = 2; i <= procs; i *= 2) {
        if (rank % i == 0) {
            int recv_val;
            printf("%d: %d receives from %d\n", i, rank, rank + (i / 2));
            MPI_Recv(&recv_val, 1, MPI_INT, rank + (i / 2), 1, MPI_COMM_WORLD, NULL);
            value += recv_val;
        } else if (rank % (i / 2) == 0) {
            printf("%d: %d sends to %d\n", i, rank, rank - (i / 2));
            MPI_Send(&value, 1, MPI_INT, rank - (i / 2), 1, MPI_COMM_WORLD);
        }
    }

    if (rank == MASTER) {
        printf("Result = %d\n", value);
    }

    MPI_Finalize();

}

