#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define num_threads 4

int var = 0;
pthread_mutex_t lock;


// race condition 

void *f( void *args){
   // pthread_mutex_t lock = *( pthread_mutex_t * )args;
  // pthread_mutex_lock(&*( pthread_mutex_t * )args);
   // pthread_mutex_lock(&lock);
    for ( int i = 0; i < 10000; i++){
        var = var + 1;
    }
   // pthread_mutex_unlock(&*( pthread_mutex_t * )args);
   // pthread_mutex_unlock(&lock);
  
    pthread_exit(NULL);
    return NULL;
}

int main(){
    
 //   pthread_mutex_t lock;
    pthread_t threads[num_threads];

    int r = pthread_mutex_init(&lock, NULL);
    if (r){
        fprintf(stderr,"eroare initializare mutex");
    }

    for( int i = 0; i < num_threads; i++){
        int id = i;
        int r = pthread_create(&threads[i],NULL,f, (void *)&lock);
        if(r){
            fprintf(stderr,"Eroare la creare thread");
        }
    }

    void *status;

    for( int i = 0; i < num_threads; i++){
        r = pthread_join(threads[i],&status);
        if(r){
            fprintf(stderr,"Eroare la join");
        }
    }
    printf("%d",var);

    pthread_mutex_destroy(&lock);
    
    return 0;
}