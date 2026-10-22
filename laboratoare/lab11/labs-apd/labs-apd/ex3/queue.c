#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

typedef struct {
    int size;
    int arr[1000];
} queue;

int main (int argc, char *argv[]) {
    int numtasks, rank;

    queue q;
    MPI_Datatype queue_type, oldtypes[2];
    int blockcounts[2];
    MPI_Aint offsets[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    offsets[0] = offsetof(queue, size);
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 1;

    offsets[1] = offsetof(queue, arr);
    oldtypes[1] = MPI_INT;
    blockcounts[1] = 1000;

    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &queue_type);
    MPI_Type_commit(&queue_type);

    
    // First process starts the circle.
    if (rank == 0) {

    	q.size = 0;
    	srand((rank + 1) * time(NULL));
        q.arr[q.size++] = rand() % 1000;
        printf("Process [%d] sends the number %d\n", rank, q.arr[q.size-1]);  
        MPI_Send(&q, 1, queue_type, rank + 1, 0, MPI_COMM_WORLD);

        MPI_Recv(&q, 1, queue_type, numtasks - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

    } else if (rank == numtasks - 1) {

        MPI_Recv(&q, 1, queue_type, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        srand((rank + 1) * time(NULL));
        q.arr[q.size++] = rand() % 1000;
        printf("Process [%d] sends the number %d\n", rank, q.arr[q.size-1]); 
        MPI_Send(&q, 1, queue_type, 0, 0, MPI_COMM_WORLD);

    } else {

        MPI_Recv(&q, 1, queue_type, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        srand((rank + 1) * time(NULL));
        q.arr[q.size++] = rand() % 1000;
      	printf("Process [%d] sends the number %d\n", rank, q.arr[q.size-1]);  
        MPI_Send(&q, 1, queue_type, rank + 1, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("Queue size = %d\n", q.size);
        printf("Elements from queue\n");
        for (int i = 0; i < q.size; i++) {
            printf("%d ", q.arr[i]);
        }
        printf("\n");
    }

    MPI_Type_free(&queue_type);

    MPI_Finalize();
}
