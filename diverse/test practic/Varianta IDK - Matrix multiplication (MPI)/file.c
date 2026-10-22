#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define nmax 101

// Functia care genereaza matricele conform cerintei
void generareMatrici(int N, int A[nmax][nmax], int B[nmax][nmax]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }
}

// Functia pentru inmultirea matricelor
void inmultireMatrici(int N, int start, int end, int A[nmax][nmax], int B[nmax][nmax], int C[nmax][nmax]) {
    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size, N;
    int A[nmax][nmax], B[nmax][nmax], C[nmax][nmax];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 2) {
        if (rank == 0) {
            printf("Utilizare: %s N\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    N = atoi(argv[1]);
    if (N > 100 || N < 1 || size > 8 || size < 1) {
        if (rank == 0) {
            printf("Input invalid. N trebuie sa fie cuprins in intervalul 1 <= N <= %d si trebuie minim 2 procese\n", nmax);
        }
        MPI_Finalize();
        return -1;
    }

    // Doar rank 0 initializeaza matricile A si B
    if (rank == 0) {
        generareMatrici(N, A, B);
    }

    // Transmitem prin broadcast catre toate rank-urile
    MPI_Bcast(B, nmax * nmax, MPI_INT, 0, MPI_COMM_WORLD);

    int rows = N / size;
    int ramas = N % size;

    // Determin pentru rank-ul curent intervalul start - end
    int start;
    if (rank < ramas) {
        start = rank * rows + rank;
    } else {
        start = rank * rows + ramas;
    }

    int end;
    if (rank < ramas) {
        end = start + rows + 1;
    } else {
        end = start + rows;
    }

    // Rank 0 trimite la fiecare rank diferit de 0 bucata de A necesara
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int start2 = i * rows + (i < ramas ? i : ramas);
            int end2 = start2 + rows + (i < ramas ? 1 : 0);

            // Trimit exact cate elemente este necesar
            MPI_Send(&A[start2][0], (end2 - start2)*N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Rank ul diferit de 0 primeste portiunea din A de la rank 0
        MPI_Recv(&A[start][0], (end - start) * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Fiecare rank calculeaza produsul pentru propriile linii
    inmultireMatrici(N, start, end, A, B, C);

    // Rank != 0 trimite partea lui din C inapoi la rank 0
    if (rank == 0) {
        // Rank 0 primeste rezultatele de la ceilalti
        for (int i = 1; i < size; i++) {
            int start3 = i * rows + (i < ramas ? i : ramas);
            int end3 = start3 + rows + (i < ramas ? 1 : 0);

            MPI_Recv(&C[start3][0], (end3 - start3) * N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Doar rank 0 afiseaza rezultatul final
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    } else {
        // Trimite partea calculata din C la rank 0
        MPI_Send(&C[start][0], (end - start) * N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
