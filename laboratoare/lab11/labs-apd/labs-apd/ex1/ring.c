#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Request send, receive;
    MPI_Status status;

    int recv_num;
    int flag;

    // First process starts the circle.
    if (rank == 0) {
        srand(42);
        int rand_num = rand() % numtasks;
        printf("Process [%d] generated number %d.\n", rank, rand_num);

        printf("Process [%d] send %d to process [%d].\n", rank, rand_num, rank + 1);
        MPI_Isend(&rand_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send);
        MPI_Test(&send, &flag, &status);

        if (flag) {
            printf("[P%d] The send operation is over\n", rank);
        } else {
            printf("[P%d] The send operation is not over yet\n", rank);
            MPI_Wait(&send, &status);
        }


        MPI_Irecv(&rand_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD, &receive);
        MPI_Test(&receive, &flag, &status);

        if (flag) {
            printf("[P%d] The receive operation is over\n", rank);
        } else {
            printf("[P%d] The receive operation is not over yet\n", rank);
            MPI_Wait(&receive, &status);
        }

        printf("Process [%d] received %d from process [%d].\n", rank, rand_num,
                numtasks - 1);


    } else if (rank == numtasks - 1) {
        int rand_num;
        MPI_Irecv(&rand_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &receive);
        MPI_Test(&receive, &flag, &status);

        if (flag) {
            printf("[P%d] The receive operation is over\n", rank);
        } else {
            printf("[P%d] The receive operation is not over yet\n", rank);
            MPI_Wait(&receive, &status);
        }

        printf("Process [%d] received %d from process [%d].\n", rank, rand_num,
                rank - 1);

        rand_num += 2;

        // Last process ends the circle by sending the value to the first.
        printf("Process [%d] send %d to process [%d].\n", rank, rand_num, 0);
        MPI_Isend(&rand_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &send);

        MPI_Test(&send, &flag, &status);

        if (flag) {
            printf("[P%d] The send operation is over\n", rank);
        } else {
            printf("[P%d] The send operation is not over yet\n", rank);
            MPI_Wait(&send, &status);
        }

    } else {
        int rand_num;
        MPI_Irecv(&rand_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &receive);
        MPI_Test(&receive, &flag, &status);

        if (flag) {
            printf("[P%d] The receive operation is over\n", rank);
        } else {
            printf("[P%d] The receive operation is not over yet\n", rank);
            MPI_Wait(&receive, &status);
        }
        printf("Process [%d] received %d from process [%d].\n", rank, rand_num,
                rank - 1);

        rand_num += 2;

        printf("Process [%d] send %d to process [%d].\n", rank, rand_num, rank + 1);
        MPI_Isend(&rand_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send);

        MPI_Test(&send, &flag, &status);

        if (flag) {
            printf("[P%d] The send operation is over\n", rank);
        } else {
            printf("[P%d] The send operation is not over yet\n", rank);
            MPI_Wait(&send, &status);
        }
    }

    MPI_Finalize();

}

