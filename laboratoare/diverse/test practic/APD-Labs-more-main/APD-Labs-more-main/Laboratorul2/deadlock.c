#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

pthread_mutex_t lock1;  
pthread_mutex_t lock2;

void *f(void *args ) {
     pthread_mutex_lock(&lock1); 
     printf("Thread 1 locked mutex1\n");
     sleep(1);
     pthread_mutex_lock(&lock2);
     printf("Thread 1 locked mutex2\n");
     pthread_mutex_unlock(&lock2); // to release the resource
     printf("Thread 1 unlocked mutex2\n");
     pthread_mutex_unlock(&lock1);
     printf("Thread 1 unlocked mutex1\n");
}

void *g(void *args ) {
     pthread_mutex_lock(&lock2);
     printf("Thread 2 locked mutex2\n");
     sleep(1);
     pthread_mutex_lock(&lock1);
     printf("Thread 2 locked mutex1\n");
     pthread_mutex_unlock(&lock1);
     printf("Thread 2 unlocked mutex1\n");
     pthread_mutex_unlock(&lock2);
     printf("Thread 2 unlocked mutex2\n");
}

int main() {

 pthread_mutex_init(&lock1,NULL); 
 pthread_mutex_init(&lock2,NULL);

 int r;
 void *status;

 pthread_t one, two;
   
 r = pthread_create(&one, NULL, f, NULL); 
    if(r){
        fprintf(stderr,"Eroare la create");
        return -1;
    }

 r = pthread_create(&two, NULL, g, NULL);
    if(r){
        fprintf(stderr,"Eroare la create");
        return -1;
    }

 r = pthread_join(one, &status);
    if(r){
        fprintf(stderr,"Eroare la join");
        return -1;
    }
  
 r = pthread_join(two, &status);
    if(r){
        fprintf(stderr,"Eroare la join");
        return -1;
    }

 }