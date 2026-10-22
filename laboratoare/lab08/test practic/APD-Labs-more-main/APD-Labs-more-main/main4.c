#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 3

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int value;

    if (rank == ROOT) {

            MPI_Status stat;
            MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &stat);
            printf("Process [%d] received %d from process [%d].\n", rank, value,
                stat.MPI_SOURCE);
             MPI_Status status;


    } else {

        srand(time(NULL));
        value = rand() % (rank * 50 + 1);
        printf("Process [%d] send %d.\n", rank, value);
        MPI_Send(&value, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
        printf("%d ",value);

    }

    MPI_Finalize();
    

}
