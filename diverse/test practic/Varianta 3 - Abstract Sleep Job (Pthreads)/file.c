#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_THREADS 8

typedef struct {
    int thread_id;
    int sleep_time;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    int* finished;
    pthread_barrier_t* barrier;
    int* finished_count;
} thread_data_t;

void* thread_function(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    
    pthread_barrier_wait(data->barrier);
    
    if (data->thread_id == 0) {
        // Monitor thread logic
        int total_to_finish = data->sleep_time - 1;  // P-1 worker threads
        while (*data->finished_count < total_to_finish) {
            pthread_mutex_lock(data->mutex);
            pthread_cond_wait(data->cond, data->mutex);
            
            // Check which thread just finished
            for (int i = 1; i < data->sleep_time; i++) {
                if (data->finished[i] == 1) {
                    data->finished[i] = 2;  // Mark as processed
                    printf("Thread-ul %d a terminat\n", i);
                    (*data->finished_count)++;
                }
            }
            pthread_mutex_unlock(data->mutex);
        }
    } else {
        // Worker thread logic
        sleep(data->sleep_time);
        
        pthread_mutex_lock(data->mutex);
        data->finished[data->thread_id] = 1;
        pthread_cond_signal(data->cond);
        pthread_mutex_unlock(data->mutex);
    }
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    
    int P = atoi(argv[1]);
    if (P < 2 || P > MAX_THREADS) return 1;
    
    pthread_t threads[MAX_THREADS];
    thread_data_t thread_data[MAX_THREADS];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    int finished[MAX_THREADS] = {0};
    int finished_count = 0;
    
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, P);
    
    // Initialize thread data
    for (int i = 0; i < P; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].sleep_time = (i == 0) ? P : atoi(argv[i + 1]);
        thread_data[i].mutex = &mutex;
        thread_data[i].cond = &cond;
        thread_data[i].finished = finished;
        thread_data[i].barrier = &barrier;
        thread_data[i].finished_count = &finished_count;
    }
    
    // Create all threads
    for (int i = 0; i < P; i++) {
        pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < P; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_barrier_destroy(&barrier);
    
    return 0;
}