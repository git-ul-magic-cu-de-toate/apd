#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int *arr, n;

void bubblesort(){

 int sorted = 0, aux;

 while(!sorted){

     sorted = 1;
     for(int i = 0; i < n; i++){

         if( arr[i] > arr[i + 1]){
             aux = arr[i + 1];
             arr[i + 1] = arr[i];
             arr[i] = aux;
             sorted = 0;
         }
     }
     
 }

}
int main(int argc, char** argv){

    scanf("%d",&n);
    
    arr = (int *) malloc(n*sizeof(n));
    if(arr == NULL){
        fprintf(stderr,"Eroare la malloc");
        return -1;
    }

    for( int i = 0; i < n; i++){
        scanf("%d",&arr[i]);
    }    

    bubblesort();

    for( int i = 0; i < n; i++){
        printf("%d ",arr[i]);
    }   

    return 0;
}
