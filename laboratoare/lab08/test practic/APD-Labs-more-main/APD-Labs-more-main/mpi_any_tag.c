#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_ELEMS   (10)

int main (int argc, char* argv[])
{
    int* v = malloc(NUM_ELEMS * sizeof(*v));

    if (v == NULL)
    {
        fprintf(stderr, "Unable to allocate the buffer.\n");
        return 0;
    }

    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(hostname, &len);

    if (rank == 0)
    {   int i;
        for (i = 0; i != NUM_ELEMS; ++i)
        {
            v[i] = i;
        }
        for(int i = 1; i < numtasks; i++){
            MPI_Send(v, NUM_ELEMS, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else    
    {
       MPI_Status status; 
       MPI_Recv(v, NUM_ELEMS, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        for (int i = 0; i != NUM_ELEMS; ++i)
        {
            printf("%d ", v[i]);
        }
      
       printf("\n");
    }

    MPI_Finalize();

    return 0;
}