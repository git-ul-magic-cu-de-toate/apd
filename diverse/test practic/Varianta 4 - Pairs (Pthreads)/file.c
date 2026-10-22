#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define VMAX 10000
#define MMAX 10
#define MMIN 2
#define NUMNUMS 10

int nums[NUMNUMS];
sem_t sems[NUMNUMS];

void *thread_function(void *arg) {
    int *id = (int *)arg;
    int m = rand() % (MMAX - MMIN + 1) + MMIN;

    while (m) {
        int i = rand() % NUMNUMS;
        int v = rand() % VMAX;
        printf("[Thread %d]: a generat perechea (%d, %d)\n", *id, i, v);
        sem_wait(&sems[i]);
        sleep(1);
        if (v > nums[i]) {
            nums[i] = v;
        }
        sem_post(&sems[i]);
        m--;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int numt = atoi(argv[1]);
    int thread_ids[numt];

    for (int i = 0; i < numt; i++) {
        thread_ids[i] = i;
    }

    for (int i = 0; i < NUMNUMS; i++) {
        nums[i] = 0;
        sem_init(&sems[i], 0, 1);
    }

    pthread_t threads[numt];

    for (int i = 0; i < numt; i++) {
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }

    for (int i = 0; i < numt; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUMNUMS; i++) {
        sem_destroy(&sems[i]);
    }

    for (int i = 0; i < 10; i++) {
        printf("(%d, %d) ", i, nums[i]);
    }
    printf("\n");

    return 0;
}