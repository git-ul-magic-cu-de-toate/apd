#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define THREADS 4

int x = 0;


void *f(void *args){
    x = 7;
    printf("%d\n",x);
}

void *g(void *args){
    x = 2;
    printf("%d\n",x);
}


int main(){

    pthread_t threads[THREADS];
    pthread_create(&threads[0], NULL,f,"---");
    pthread_create(&threads[1],NULL,g,"----");
    int r;
    void *status;

    r = pthread_join(threads[0],&status);
    if( r ){
        fprintf(stderr,"Error");
    }
    r = pthread_join(threads[1],&status);
    if( r ){
        fprintf(stderr,"Error");
    }

    return 0;
}