#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

long gSum = 0;
pthread_mutex_t lk; 
sem_t s;

int main(int argc, char**argv) {
    //add pthread error checking
    if(argc != 2){
        printf("Incorrect number of command line arguements");
        return 1;
    }
    sem_init(&s,0,0); //0 or 1 as last param?
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);

    pthread_t threads[m]; 
    for(int i = 0; i < m; i++){
        //change this to be dependent on iteration
        //rename this struct probly yes do this
        //we need proper allocation here!
        sumSquareRootsArgs* parametersCurrent = {(i*(n/m)+1,(i+1)*(n/m))};
        pthread_create(&threads[i],NULL,sumOfSqaureoots,(void *)parametersCurrent);
        
    }
    //cannot use pthread join here

    //semaphore sync
    for (int i = 0; i < m; i++){
        sem_wait(&s); //check if this is right
    }

    //What should this say? I don't think this is the place to do it
    printf("Sum: %f",gSum);

    //clean up
    sem_destroy(&s);
    pthread_mutex_destroy(&lk);
    return 0;
}

void* sumOfSqaureoots(void* parameters){
    sumSquareRootsArgs* params = (sumSquareRootsArgs*)(parameters);
    long squareRootSum = 0;
    for(int i = params->start; i < params->end; i++){
        squareRootSum += sqrt(i);
    }

    //safety stuff to prevent race conditions
    pthread_mutex_lock(&lk);
    gSum = squareRootSum;
    pthread_mutex_unlock(&lk);

    //semaphore stuff
    sem_post(&s);
}

//this could also be an enum
//Should params be type int?
//We need to pass in the tid of the thread for debugging
typedef struct{
    int start;
    int end;
}sumSquareRootsArgs;