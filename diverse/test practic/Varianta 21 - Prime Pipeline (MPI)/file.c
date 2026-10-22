#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int is_prime(int num) {
    if (num < 2) {
        return 0;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}


int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // vector de numere prime de size, citite de rank 1
    int primes[size];
    if (rank == 0) {
        // rank 0 citeste inputul
        for (int i = 0; i < size; i++) {
            primes[i] = atoi(argv[i + 1]);
        }

        // trimite vectorul de numere prime catre rank 1
        primes[rank] = is_prime(primes[rank]) ? primes[rank] : 0;
        MPI_Send(primes, size, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    if (rank > 0 && rank < size - 1) {
        // citeste vectorul de nr
        MPI_Recv(primes, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // modifica partea lui de vector
        primes[rank] = is_prime(primes[rank]) ? primes[rank] : 0;

        // trimite mai departe
        MPI_Send(primes, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    if (rank == size - 1) {
        // citeste vectorul de nr
        MPI_Recv(primes, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // modifica partea lui de vector
        primes[rank] = is_prime(primes[rank]) ? primes[rank] : 0;

        // printeaza nr prime
        for (int i = 0; i < size; i++) {
            if (primes[i] != 0) {
                printf("%d ", primes[i]);
            }
        }
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}