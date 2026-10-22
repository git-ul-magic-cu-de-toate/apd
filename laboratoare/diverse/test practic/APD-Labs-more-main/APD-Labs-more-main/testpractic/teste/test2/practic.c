#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define CONVERGENCE_COEF 100
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MASTER (0)
#define MAX_DIST 0xFFFF
/**
 * Run: mpirun -np 12 ./a.out
 */

static int num_neigh;
static int *neigh;
static int adj_matrix[100][100];
static int n;
static int m;

void read_neighbours() {
    FILE *fp;

    fp = fopen("test2.txt", "r");
	fscanf(fp, "%d", &n);
	fscanf(fp, "%d", &m);

	int start, end;

	for (size_t i = 0; i < m; ++i) {
		fscanf(fp, "%d%d", &start, &end);
		adj_matrix[start][end] = 1;
        adj_matrix[end][start] = 1;
	}    
}

int leader_chosing(int rank, int nProcesses) {
	int leader = -1;
	int q;
	leader = rank;
    int current_neigh = 0;
    int max_neigh = 0;

    for (int i = 0; i < n; ++i) {
        if (adj_matrix[rank][i] == 1) {
            ++current_neigh;
        }
    }

	for (int k = 0; k < CONVERGENCE_COEF; k++) {
	    for (int i = 0; i < n; ++i) {
            if (adj_matrix[rank][i] == 1) {
                MPI_Send(&leader, 1, MPI_INT, i, MPI_TAG_UB, MPI_COMM_WORLD);
                MPI_Send(&current_neigh, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                int neighbour_leader = 0;
                int recv_no_neigh = 0;
                MPI_Recv(&neighbour_leader, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&recv_no_neigh, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if ((leader < neighbour_leader) && (current_neigh < recv_no_neigh)) {
                    leader = neighbour_leader;
                    max_neigh = recv_no_neigh;
                }
            }
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	printf("%i/%i: leader is %i %i\n", rank, nProcesses, leader, max_neigh);

	return leader;
}

int get_dist(int rank, int nProcesses, int k) {
	int * dist = malloc(sizeof(int) * n);
	int * recv_dist = malloc(sizeof(int) * n);

    for (int i = 0; i < n; i++) {
        dist[i] = MAX_DIST;
    }

    dist[rank] = 0;

    for (int i = 0; i < n; i++) {
        if (adj_matrix[rank][i] == 1)
            dist[i] = 1;
    }
    
    for (int k = 0; k < CONVERGENCE_COEF; k++) {
        for (int i = 0; i < n; i++) {
            if (adj_matrix[rank][i] == 1) {
                MPI_Send(dist, n, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_dist, n, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int j = 0; j < n; j++) {
                    dist[j] = MIN(dist[j], 1 + recv_dist[j]);
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        if (dist[i] == k) {
            return 1;
        }
    }

	return 0;
}

int main(int argc, char * argv[]) {
    int rank, nProcesses, num_procs, leader;
	int *parents, **topology;
    int n, m;

	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    read_neighbours();

    leader = leader_chosing(rank, nProcesses);

	MPI_Barrier(MPI_COMM_WORLD);
    int k = 3;
    if (get_dist(rank, nProcesses, k)) {
        printf("da\n");
    } else {
        printf("nu\n");
    }
    MPI_Finalize();
	return 0;
}