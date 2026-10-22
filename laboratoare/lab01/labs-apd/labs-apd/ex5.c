#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int* arr;
int number_of_threads;
int array_size;

int min(int a, int b) {
    return a <= b ? a : b;
}

void *add(void *arg) {
  	int thread_id = (int)arg;
    int start_index = thread_id * ((double)array_size / number_of_threads);
    int end_index = min(((thread_id + 1) * ((double)array_size / number_of_threads)), array_size);

    for (int i = start_index; i < end_index; i++) {
        arr[i] += 100;
    }

  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati numarul de thread-uri si dimensiunea array-ului\n");
        exit(-1);
    }

    number_of_threads = atoi(argv[1]);
    array_size = atoi(argv[2]);

	pthread_t threads[number_of_threads];
  	int r;
  	long id;
  	void *status;

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	for (id = 0; id < number_of_threads; id++) {
		r = pthread_create(&threads[id], NULL, add, (void *)id);
		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	for (id = 0; id < number_of_threads; id++) {
		r = pthread_join(threads[id], &status);
		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

      for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	pthread_exit(NULL);
}
