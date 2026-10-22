#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int min(int a, int b) {
    return a < b ? a : b;
}

int* array;
int* result;
int NUM_THREADS;
int NUM_ELEMENTS;

pthread_barrier_t barrier;

// Thread function: it could perform any desired task.
void* thread_function(void* arg) {
    int thread_id = *(int *)arg;
    int start_index = thread_id * ((double)NUM_ELEMENTS / NUM_THREADS);
    int end_index = min(((thread_id + 1) * ((double)NUM_ELEMENTS / NUM_THREADS)), NUM_ELEMENTS);
    
    printf("Thread %d is processing elements from %d to %d.\n", thread_id, start_index, end_index);
    for (int i = start_index; i < end_index; i++) {
        result[i] = 0;
        for (int j = 0; j <= i; j++) {
            result[i] += array[j];
        }
    }

    // Wait for all threads to finish
    pthread_barrier_wait(&barrier);

    if (thread_id == 0) {
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
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

    result = (int *)malloc(NUM_ELEMENTS * sizeof(int));

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
    
    printf("All threads completed execution.\n");

    return EXIT_SUCCESS;
}
