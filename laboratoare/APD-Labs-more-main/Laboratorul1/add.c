#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int* arr;
int array_size;

typedef struct Pair {
    int start;
    int end;
} Pair;

void* add(void *arg) {
    Pair *p = (Pair *) arg;
    
    for (int i = p->start ; i < p->end ; ++i) {
        arr[i] += 100;
    }

    free(p);
    pthread_exit(NULL);
    return NULL;
}

int min(int a, int b) { 
    return a < b ? a : b;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(*arr));

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
    

  	long cores = sysconf(_SC_NPROCESSORS_CONF);
    pthread_t threads[cores];
    int r;

    for (int i = 0 ; i < cores ; ++i) {
        Pair *p = malloc(sizeof(*p));
        p->start = i * ((double) array_size / cores);
        p->end = min((i + 1) * ((double) array_size / cores), array_size);
        r = pthread_create(&threads[i], NULL, add, (void *)p);        
        
        if (r) {
	  		    printf("Eroare la asteptarea thread-ului %d\n", i);
	  		    exit(-1);
		    }
    }

    for (int i = 0 ; i < cores ; ++i) {
       r = pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }
    
    free(arr);
  	pthread_exit(NULL);

    return 0;
}