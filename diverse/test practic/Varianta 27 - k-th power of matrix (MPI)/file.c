#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void multiply_matrices(int N, int* A, int* B, int* C) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] = 0;
            for (int k = 0; k < N; k++) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

void copy_matrix(int N, int* source, int* dest) {
    for (int i = 0; i < N * N; i++) {
        dest[i] = source[i];
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Read command line arguments
    int N = atoi(argv[1]);  // Matrix size
    int k = atoi(argv[2]);  // Power

    // Allocate matrices
    int* matrix = (int*)malloc(N * N * sizeof(int));
    int* result = (int*)malloc(N * N * sizeof(int));
    int* temp = (int*)malloc(N * N * sizeof(int));
    
    // Initialize matrix in rank 0
    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i * N + j] = i + j;
            }
        }
        // For k=1, result is the matrix itself
        copy_matrix(N, matrix, result);
    }

    // Broadcast initial matrix to all processes
    MPI_Bcast(matrix, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(result, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate how many rows each process will handle
    int rows_per_proc = N / size;
    if (rank < N % size) rows_per_proc++;
    
    // Local matrices for partial computations
    int* local_result = (int*)malloc(rows_per_proc * N * sizeof(int));
    
    // Matrix multiplication for power k
    for (int p = 2; p <= k; p++) {
        int start_row = 0;
        for (int i = 0; i < rank; i++) {
            start_row += N / size;
            if (i < N % size) start_row++;
        }
        
        // Each process computes its portion of rows
        for (int i = 0; i < rows_per_proc; i++) {
            for (int j = 0; j < N; j++) {
                local_result[i * N + j] = 0;
                for (int k = 0; k < N; k++) {
                    local_result[i * N + j] += result[(start_row + i) * N + k] * matrix[k * N + j];
                }
            }
        }

        // Gather results back to rank 0
        int* recv_counts = NULL;
        int* displs = NULL;
        if (rank == 0) {
            recv_counts = (int*)malloc(size * sizeof(int));
            displs = (int*)malloc(size * sizeof(int));
            int offset = 0;
            for (int i = 0; i < size; i++) {
                int rows = N / size;
                if (i < N % size) rows++;
                recv_counts[i] = rows * N;
                displs[i] = offset * N;
                offset += rows;
            }
        }

        MPI_Gatherv(local_result, rows_per_proc * N, MPI_INT,
                    temp, recv_counts, displs, MPI_INT,
                    0, MPI_COMM_WORLD);

        if (rank == 0) {
            copy_matrix(N, temp, result);
            free(recv_counts);
            free(displs);
        }

        // Broadcast the new result matrix to all processes
        MPI_Bcast(result, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Print result from rank 0
    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d", result[i * N + j]);
                if (j < N - 1) printf(" ");
            }
            printf("\n");
        }
    }

    // Cleanup
    free(matrix);
    free(result);
    free(temp);
    free(local_result);

    MPI_Finalize();
    return 0;
}