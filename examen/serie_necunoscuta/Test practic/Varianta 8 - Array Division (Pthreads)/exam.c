#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int NUM_ELEMENTS;
int NUM_THREADS;

int* numbers;
int** result;
int* sizes;
pthread_barrier_t barrier;
pthread_mutex_t mutex;

// Thread function: it could perform any desired task.
void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    int divisor = thread_id + 2;  // The divisor for this thread

    // Temporary array to store the numbers divisible by the divisor
    int* temp = (int*)malloc(sizeof(int) * NUM_ELEMENTS);
    int temp_size = 0;

    // Check which numbers are divisible by the divisor
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        if (numbers[i] % divisor == 0) {
            // Store the number in the temporary array
            temp[temp_size++] = numbers[i];
        }
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < temp_size; i++) {
        result[thread_id][i] = temp[i];
    }
    sizes[thread_id] = temp_size;
    pthread_mutex_unlock(&mutex);

    // Free the temporary array
    free(temp);

    // Wait for all threads to finish their job before printing
    pthread_barrier_wait(&barrier);

    // Only thread 0 prints the results
    if (thread_id == 0) {
        for (int i = 2; i <= NUM_THREADS + 1; i++) {
            printf("%d: ", i);
            for (int j = 0; j < sizes[i - 2]; j++) {
                printf("%d ", result[i - 2][j]);
            }
            printf("\n");
        }
    }
    
    // Exit the thread
    pthread_exit(NULL);
}

int main(char argc, char* argv[]) {
    // Get the number of elements and threads from the command line
    NUM_ELEMENTS = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    pthread_t threads[NUM_THREADS];
    int thread_id[NUM_THREADS];

    // Allocate memory for the numbers
    numbers = (int*)malloc(NUM_ELEMENTS * sizeof(int));
    
    // Read the numbers from the command line
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        numbers[i] = atoi(argv[i + 3]);
    }

    // Allocate memory for the result
    result = (int**)malloc(sizeof(int*) * NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        result[i] = (int*)malloc(sizeof(int) * NUM_ELEMENTS);
    }

    // Initialize the sizes
    sizes = (int*)malloc(sizeof(int) * NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        sizes[i] = 0;
    }

    // Initialize the barrier
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    pthread_mutex_init(&mutex, NULL);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, thread_function, (void *)&thread_id[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Free memory
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    free(sizes);
    for (int i = 0; i < NUM_THREADS; i++) {
        free(result[i]);
    }
    free(result);
    free(numbers);

    return EXIT_SUCCESS;
}
