#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct args {
    char* name;
    int age;
};

void *hello(void *input) {
    struct args st = *(struct args*)input;
    printf("name: %s\n", ((struct args*)input)->name);
    printf("age: %d\n", ((struct args*)input)->age);
    st.age++;
    printf("%d\n",st.age);

    pthread_exit(NULL);
    return NULL;

}
void *hello1(void *input) {
    struct args st = *(struct args*)input;
    printf("name: %s\n", ((struct args*)input)->name);
    printf("age: %d\n", ((struct args*)input)->age);
    st.age = st.age + 100;
    printf("%d\n",st.age);

    pthread_exit(NULL);
    return NULL;

}

int main() {

    struct args *Allen = (struct args *)malloc(sizeof(struct args));
    char allen[] = "Allen";
    Allen->name = allen;
    Allen->age = 20;

    pthread_t tid, tidd;

    pthread_create(&tid, NULL, hello, (void *)Allen);
    pthread_create(&tidd, NULL, hello1, (void *)Allen);

    pthread_join(tid, NULL);
    pthread_join(tidd, NULL);
    
    printf("%d",Allen->age);
    return 0;
}