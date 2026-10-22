#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MULTI 5 // chunk dimension
#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int num_elements = MULTI * numtasks;
    int *v_send = NULL;
    int *v_recv = (int *)malloc(MULTI * sizeof(int));

    if (rank == ROOT) {

        v_send = (int *)malloc(num_elements * sizeof(int));

        for (int i = 0; i < num_elements; ++i) {
            v_send[i] = i;
        }

    }

    MPI_Scatter(v_send, MULTI, MPI_INT, v_recv, MULTI, MPI_INT, ROOT,
                MPI_COMM_WORLD);

    printf("Process [%d]: have elements %d %d %d %d %d.\n", rank, v_recv[0],
            v_recv[1], v_recv[2], v_recv[3], v_recv[4]);

    for (int i = 0; i < MULTI; ++i) {
        v_recv[i] += rank;
    }

    MPI_Gather(v_recv, MULTI, MPI_INT, v_send, MULTI, MPI_INT, ROOT,
               MPI_COMM_WORLD);

    if (rank == ROOT) {
        for (int i = 0; i < num_elements; ++i) {
            printf("%d ", v_send[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

}
