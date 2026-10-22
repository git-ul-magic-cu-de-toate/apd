#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2

void *f1(void *arg) {
  	printf("Hello World din thread-ul 0!\n");
  	pthread_exit(NULL);
}

void *f2(void *arg) {
  	printf("Hello World din thread-ul 1!\n");
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
  	int r, r1, r2;
  	long id;
  	void *status;

	r1 = pthread_create(&threads[0], NULL, f1, NULL);
	if (r1) {
	  	printf("Eroare la crearea thread-ului 0\n");
	  	exit(-1);
	}

	r2 = pthread_create(&threads[1], NULL, f2, NULL);
	if (r2) {
	  	printf("Eroare la crearea thread-ului 1\n");
	  	exit(-1);
	}

  	for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
