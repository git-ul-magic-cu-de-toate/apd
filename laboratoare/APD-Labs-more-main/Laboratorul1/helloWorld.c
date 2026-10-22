#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define num_threads 4

int var = 1;

void *f(void *argss){
   int a = *(int *)argss;
    printf(" hello world from %d - %d\n", a,var);


    pthread_exit(NULL);
    return NULL;
}
 
int main(){

    pthread_t threads[num_threads];
    int args[num_threads];
    int r;
    void *status;

  for ( int i = 0; i < num_threads; i++){
      args[i] = i;
      r =  pthread_create(&threads[i],NULL,f,(void *) &args[i]);
  }

  for ( int i = 0; i < num_threads; i++){
      r = pthread_join(threads[i], &status);
   }
   return 0;

}
