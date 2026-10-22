#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void odd_even_sort(int rank, int size, int *value) {
    int neighbor_value;
    MPI_Status status;

    for (int i = 0; i < size; i++) {
        if (i % 2 == 0) {
            // Phase 1: Even phase
            if (rank % 2 == 0 && rank + 1 < size) {
                // Even-ranked task compares with the right neighbor
                MPI_Send(value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&neighbor_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);

                if (*value > neighbor_value) {
                    // Swap values
                    *value = neighbor_value;
                }
            } else if (rank % 2 == 1) {
                // Odd-ranked task compares with the left neighbor
                MPI_Recv(&neighbor_value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
                MPI_Send(value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

                if (*value < neighbor_value) {
                    // Keep the maximum value
                    *value = neighbor_value;
                }
            }
        } else {
            // Phase 2: Odd phase
            if (rank % 2 == 1 && rank + 1 < size) {
                // Odd-ranked task compares with the right neighbor
                MPI_Send(value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&neighbor_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);

                if (*value > neighbor_value) {
                    // Swap values
                    *value = neighbor_value;
                }
            } else if (rank % 2 == 0 && rank > 0) {
                // Even-ranked task compares with the left neighbor
                MPI_Recv(&neighbor_value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
                MPI_Send(value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

                if (*value < neighbor_value) {
                    // Keep the maximum value
                    *value = neighbor_value;
                }
            }
        }
    }
}

void collect_and_print_sorted_values(int rank, int size, int value) {
    int sorted_values[size];
    sorted_values[rank] = value;
    if (rank == size - 1) {
        for (int i = 0; i < size - 1; i++) {
            sorted_values[i] = 0;
        }
        MPI_Send(sorted_values, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    } else if (rank == 0) {
        MPI_Recv(sorted_values, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sorted_values[rank] = value;
        for (int i = 0; i < size; i++) {
            printf("%d ", sorted_values[i]);
        }
        printf("\n");
    } else {
        MPI_Recv(sorted_values, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sorted_values[rank] = value;
        MPI_Send(sorted_values, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initialize the value for each task
    int value = 2 * size - 2 * rank;

    // Perform odd-even transposition sort
    odd_even_sort(rank, size, &value);

    // Print final value after sorting
    printf("%d - %d\n", rank, value);

    // Collect and print sorted values at rank 0
    collect_and_print_sorted_values(rank, size, value);

    MPI_Finalize();
    return 0;
}
