#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>

#define num_threads 2


int *arr, N, P;
pthread_barrier_t barrier;

int min(int a, int b) {
	return a < b ? a : b;
}

void *f(void *args){
 
 int ID = *(int *) args;
 
 
 int start = ID * (double)N / P;
 int end = min((ID + 1) * (double)N / P, N);

    if (end > N) {
		end = N;
	}

 int start_odd = start & 1 ? start : start + 1;
 int start_even = start & 1 ? start + 1 : start;
 int end_odd = end & 1 ? end : end + 1;
 int end_even = end & 1 ? end + 1 : end;

 for( int k = 0; k < N; k++){
    // faza para
   for( int i = start_even; i < end_even  && i < N - 1; i = i + 2){
       if( arr[i] > arr[i + 1]){
           int aux = arr[i + 1];
           arr[i + 1] = arr[i];
           arr[i] = aux;
       }
   }
  pthread_barrier_wait(&barrier);
   // faza impara
    for( int i = start_odd; i < end_odd  && i < N - 1; i = i + 2){
       if( arr[i] > arr[i + 1]){
           int aux = arr[i + 1];
           arr[i + 1] = arr[i];
           arr[i] = aux;
       }
   }
   pthread_barrier_wait(&barrier);

 }
pthread_exit(NULL);

}

int main(){

pthread_t threads[num_threads];
int r;
void *status;

P = num_threads;
scanf("%d",&N);
int thread[N];
pthread_barrier_init(&barrier, NULL, P);

arr = (int *) malloc(N*sizeof(int));
if(arr == NULL){
    fprintf(stderr,"Eroare la malloc");
}

for( int i =0; i < N ;i ++){
    scanf("%d",&arr[i]);
}

for( int i = 0; i < P; i++){
    
    thread[i] = i;
    r = pthread_create(&threads[i],NULL,f,&thread[i]);
    if(r){
        fprintf(stderr,"Eroare la crearre de thread");
    }
}
for( int i = 0; i < P; i++){

    r = pthread_join(threads[i],&status);
    if(r){
        fprintf(stderr,"Eroare la join de thread");
    }
}
for(int i = 0; i < N; i++){
    printf("%d ",arr[i]);
}

return 0;

}