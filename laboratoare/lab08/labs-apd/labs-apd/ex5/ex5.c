#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    int send_numbers = 10;

    if (rank == 0) {

        int number;
        int tag;
        srand(42);
        for (int i = 0; i < send_numbers; ++i) {
            number = rand();
            tag = rand() % 100;

            MPI_Send(&number, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);

            printf("Process [%d] send %d with status %d.\n", rank, number, tag);
        }
    } else {

        MPI_Status status;
        int number;

        for (int i = 0; i < send_numbers; ++i) {
            MPI_Recv(&number, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
                     &status);

            printf("Process [%d] received %d with status %d.\n", rank, number,
                    status.MPI_TAG);
        }

    }

    MPI_Finalize();

}

