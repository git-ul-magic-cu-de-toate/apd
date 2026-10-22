#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int N;
int P;
int *v;
int *vInter;
pthread_barrier_t barrier;


void display_vector(int *v) {
	int i;
	int display_width = 2 + log10(N);

	for (i = 0; i < N; i++) {
		printf("%d ", v[i]);
	}

	printf("\n");
}

void get_args(int argc, char **argv)
{
	N = atoi(argv[1]);
	P = atoi(argv[2]);

	if(argc != 3 + N) {
		printf("BAD");
	}

	printf("%d %d\n", N, P);
	v = malloc(sizeof(int) * N);
	vInter = malloc(sizeof(int) * N);
    for(int i = 0; i < N; i++) {
        v[i] = atoi(argv[3 + i]);
    }
}

void init()
{
	int i;
	v = malloc(sizeof(int) * N);
    vInter = malloc(N * sizeof(int));

	if (v == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}
}

void print()
{
	printf("v: ");
	display_vector(v);
}

int min(int a, int b) {
  return a < b ? a : b;
}

void *thread_function(void *arg)
{
	int thread_id = *(int *)arg;

	// TODO: implementati aici OETS paralel
	int start = thread_id * (double)N / P;
  	int end = min((thread_id + 1) * (double)N / P, N);

	for (int k = 2; k <= N; k *= 2) {
		for (int i = start; i < end; i++) {
            vInter[i] = v[i];
        }
        pthread_barrier_wait(&barrier);

        for(int i = start; i < end; i++) {
			if (i % k == 0) {
				v[i] += vInter[i + (k / 2)];
			}
        }

		pthread_barrier_wait(&barrier);
	}

    if (thread_id == 0) {
        print();
    }


	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	get_args(argc, argv);

	int i, aux;
	pthread_t tid[P];
	int thread_id[P];
	pthread_barrier_init(&barrier, NULL, P);

	// se creeaza thread-urile
	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	// se asteapta thread-urile
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	free(v);
    free(vInter);
	pthread_barrier_destroy(&barrier);

	return 0;
}
