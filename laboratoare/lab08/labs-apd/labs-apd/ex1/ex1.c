#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int recv_num;

    // First process starts the circle.
    if (rank == 0) {
        srand(42);
        int rand_num = rand() % numtasks;
        printf("Process [%d] generated number %d.\n", rank, rand_num);

        printf("Process [%d] send %d to process [%d].\n", rank, rand_num, rank + 1);
        MPI_Send(&rand_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

        MPI_Recv(&rand_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Process [%d] received %d from process [%d].\n", rank, rand_num,
                numtasks - 1);

    } else if (rank == numtasks - 1) {

        int rand_num;
        MPI_Recv(&rand_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Process [%d] received %d from process [%d].\n", rank, rand_num,
                rank - 1);

        rand_num += 2;

        // Last process ends the circle by sending the value to the first.
        printf("Process [%d] send %d to process [%d].\n", rank, rand_num, 0);
        MPI_Send(&rand_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    } else {

        int rand_num;
        MPI_Recv(&rand_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        printf("Process [%d] received %d from process [%d].\n", rank, rand_num,
                rank - 1);

        rand_num += 2;

        printf("Process [%d] send %d to process [%d].\n", rank, rand_num, rank + 1);
        MPI_Send(&rand_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

}

