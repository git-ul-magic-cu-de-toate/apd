#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int NUM_THREADS;
int NUM_ELEMENTS;

int* array;
int* result;

pthread_barrier_t barrier;

// Thread function: it could perform any desired task.
void* thread_function(void* arg) {
    int thread_num = *(int *)arg;
    if (thread_num == 0) {
        int res = 0;
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            res += array[i];
        }
        result[thread_num] = res;
    } else if (thread_num == 1) {
        int res = 1;
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            res *= array[i];
        }
        result[thread_num] = res;
    } else if (thread_num == 2) {
        int res = 101;
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            if (array[i] < res) {
                res = array[i];
            }
        }
        result[thread_num] = res;
    } else if (thread_num == 3) {
        int res = -1;
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            if (array[i] > res) {
                res = array[i];
            }
        }
        result[thread_num] = res;
    } else if (thread_num == 4) {
        int res = 0;
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            res += array[i];
        }

        res /= NUM_ELEMENTS;
        result[thread_num] = res;
    }

    pthread_barrier_wait(&barrier);

    if (thread_num == 0) {
        for (int i = 0; i < NUM_THREADS; i++) {
            if (i == 0) {
                printf("sum: %d\n", result[i]);
            } else if (i == 1) {
                printf("prod: %d\n", result[i]);
            } else if (i == 2) {
                printf("min: %d\n", result[i]);
            } else if (i == 3) {
                printf("max: %d\n", result[i]);
            } else if (i == 4) {
                printf("avg: %d\n", result[i]);
            }
        }
    }
    
    // Exit the thread
    pthread_exit(NULL);
    // return NULL;  // Alternatively, returning NULL implicitly ends the thread.
}

int main(int argc, char *argv[]) {
    NUM_ELEMENTS = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    // Allocate memory for the array
    array = (int *)malloc(NUM_ELEMENTS * sizeof(int));
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        array[i] = atoi(argv[i + 3]);
    }

    // Allocate memory for the result
    result = (int *)malloc(NUM_THREADS * sizeof(int));

    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i;
    int rc;

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    // Create threads
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        rc = pthread_create(&threads[i], NULL, thread_function, (void *)&thread_args[i]);
        if (rc) {
            fprintf(stderr, "Error: pthread_create() failed with code %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return EXIT_SUCCESS;
}
