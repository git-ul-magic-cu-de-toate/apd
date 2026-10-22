#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int count_ones(int num) {
    int count = 0;
    while (num) {
        if (num % 10 == 1) {
            count++;
        }
        num /= 10;
    }
    return count;
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != size + 1) {
        if (rank == 0) {
            fprintf(stderr, "Numărul de argumente nu se potrivește cu numărul de task-uri.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int num = atoi(argv[rank + 1]);
    int sorted_num = num;
    int sorted_ones = count_ones(num);

    for (int step = 0; step < size - 1; step++) {
        int neighbor;
        if (rank % 2 == step % 2) {
            if (rank < size - 1) {
                neighbor = rank + 1;
                int neighbor_num;
                MPI_Sendrecv(&sorted_num, 1, MPI_INT, neighbor, 0,
                             &neighbor_num, 1, MPI_INT, neighbor, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int neighbor_ones = count_ones(neighbor_num);
                if (sorted_ones > neighbor_ones || (sorted_ones == neighbor_ones && sorted_num > neighbor_num)) {
                    sorted_num = neighbor_num;
                    sorted_ones = neighbor_ones;
                }
            }
        } else {
            if (rank > 0) {
                neighbor = rank - 1;
                int neighbor_num;
                MPI_Sendrecv(&sorted_num, 1, MPI_INT, neighbor, 0,
                             &neighbor_num, 1, MPI_INT, neighbor, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int neighbor_ones = count_ones(neighbor_num);
                if (sorted_ones < neighbor_ones || (sorted_ones == neighbor_ones && sorted_num < neighbor_num)) {
                    sorted_num = neighbor_num;
                    sorted_ones = neighbor_ones;
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == size - 1) {
        int* sorted_array = (int*)malloc(size * sizeof(int));
        MPI_Gather(&sorted_num, 1, MPI_INT, sorted_array, 1, MPI_INT, size - 1, MPI_COMM_WORLD);
        for (int i = 0; i < size; i++) {
            printf("%d ", sorted_array[i]);
        }
        printf("\n");
        free(sorted_array);
    } else {
        MPI_Gather(&sorted_num, 1, MPI_INT, NULL, 1, MPI_INT, size - 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
