#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *f(void *arg) {
  	long id = (long)arg;
	for (int i = 0; i < 100; i++) {
  		printf("Hello World din thread-ul %ld, iteratia %d!\n", id, i);
	}
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
	if (num_threads < 0) {
		exit(EXIT_FAILURE);
	}

	pthread_t threads[num_threads];
  	int r;
  	long id;
  	void *status;

	printf("Number of cores: %d\n", num_threads);

  	for (id = 0; id < num_threads; id++) {
		r = pthread_create(&threads[id], NULL, f, (void *)id);

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	for (id = 0; id < num_threads; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
