#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>



#define ROOT 0
#define NUM_ELEMS   (10)

int main (int argc, char* argv[])
{
   int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    MPI_Get_processor_name(hostname, &len);

    int rand_num;

    if (rank == ROOT) {
        srand(42);
        rand_num = rand() % numtasks;
    }

    // Broadcasts to all processes.
    MPI_Bcast(&rand_num, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    printf("Process [%d], after broadcast %d.\n", rank, rand_num);

    MPI_Finalize();
}