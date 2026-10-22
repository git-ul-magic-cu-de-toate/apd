#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

#define THREADS 8


sem_t mutex;

void* thread(void* args) {
    int *id = (int *) args; 
    sem_wait(&mutex);
    printf("Threadul a ajuns cu id %d\n",id);
    sleep(2);
    sem_post(&mutex);

    pthread_exit(NULL);
    return NULL;
 
}

int main(void) {

  pthread_t t[THREADS];
  sem_init(&mutex, 0, 3);
  
  int rr;
  for(int i = 0; i < THREADS; i++){
     rr = pthread_create(&t[i], NULL, thread, (void*)i);
     if (rr){
         fprintf(stderr,"Eroare la creearea thread-urilor");
     }
  }

  void *status;
  int r;
    for(int i = 0; i < THREADS; i++){
     r = pthread_join(t[i], &status);
     if (r){
         fprintf(stderr,"Eroare la join thread-urilor");
     }
  }
    return 0;
    
}