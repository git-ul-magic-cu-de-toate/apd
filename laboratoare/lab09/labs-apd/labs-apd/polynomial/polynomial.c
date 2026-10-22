#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define MASTER 0

int main(int argc, char * argv[]) {
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

	if (rank == MASTER) { // This code is run by a single process
		int polynomialSize, n;
		int x = 5;

		FILE * polFunctionFile = fopen(argv[1], "rt");
		fscanf(polFunctionFile, "%d", &polynomialSize);
		float *a = malloc(sizeof(float)*polynomialSize);
		for (int i = 0; i < polynomialSize; i++) {
			fscanf(polFunctionFile, "%f", &a[i]);
			printf("Read value %f\n", a[i]);
			if (i > 0) {
				printf("Sending %f to process %d\n", a[i], i);
				MPI_Send(&a[i], 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			}
		}

		fclose(polFunctionFile);

		MPI_Send(&x, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		MPI_Send(&a[0], 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
	} else {
		float val = 0, sum;
		int x;

		MPI_Recv(&val, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, NULL);
		printf("Process %d received value %f\n", rank, val);

		MPI_Recv(&x, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, NULL);
		MPI_Recv(&sum, 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, NULL);
		printf("Received x = %d and sum = %f\n", x, sum);
		sum += val * pow(x, rank);

		if (rank == nProcesses - 1) {
			printf("Polynom value is %f\n", sum);
		} else {
			MPI_Send(&x, 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
			MPI_Send(&sum, 1, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}
