#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>

/******************************************************************************
 * Program Name: thr_atomic
 * Authors: Ray Feingold 2804053, Matthew Kovach 2824267
 * Course: CIS345 Section 50
 * Date: 02/14/2025
 * Description: Utilize parallel processing among 'm' number of threads to calculate
 * the sum of square roots from 1 to 'n'
 *****************************************************************************/

// initialize global square root sum, mutex, and semaphore to share among 'm' number of threads
double gsum = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t s;

/**
 * @brief This struct is used by threads to store the range of integers a thread will
 * caluclate the partial sum of square roots of. 
 * @struct pthread_sqrt_t
 * @param l_bound integer range lower bound
 * @param u_bound integer range upper bound
 */
typedef struct{
    int tid; /**< pthread id */
    int l_bound; /**< square root sum lower bound */
    int u_bound; /**< square root sum upper bound */
} pthread_sqrt_t;

/**
 * @brief Computes the sum of square roots from the lower to upper bound of a pthread_sqrt_t
 * @param thread_args The pthread_sqrt_t passed during a pthread_create call
 */
void *calc(void *thread_args){
    // create local pthread_sqrt_t variable received from pthread_create args
    pthread_sqrt_t *thread_m = (pthread_sqrt_t*)thread_args;

    // initialize local thread partial square root sum
    double lsum = 0;

    // calculate sum of square roots from the upper and lower bounds of struct pthread_sqrt_t
    for(int i = thread_m->l_bound; i <= thread_m->u_bound; i++){
        lsum += sqrt(i);
    }

    /**
     * to prevent race conditions during parallel thread processing,
     * we wrap the global variable gsum with a pthread mutex
     * this area is known as the 'critical region'
     */

    pthread_mutex_lock(&lock);
    gsum += lsum;
    printf("thr[%d]: %.6f\n", thread_m->tid, lsum);
    pthread_mutex_unlock(&lock);

    // clean up, perform semaphore synchronization, and exit thread
    free(thread_m);
    sem_post(&s);
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    // check if m & n are provided in command-line arguments
    if(argc != 3){
        printf("\nIncorrect number of command-line arguments provided\n");
        return 1;
    }

    // initialize semaphore 
    sem_init(&s, 0, 0);

    // from command-line args-> 'm' = number of threads, 'n' = integer range upper bound
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);

    /**
     * create 'm' number of threads to calculate partial square root sums
     * between the determined lower to upper bound integer range
     * of current thread_m
     */
    pthread_t thr[m];
    for(int i = 0; i < m; i++){
        pthread_sqrt_t *thread_m = malloc(sizeof(pthread_sqrt_t));
        thread_m->tid = i;
        thread_m->l_bound = i * (n / m) + 1;
        thread_m->u_bound = (i + 1) * (n / m);
        
        // check if successful thread creation
        int t = pthread_create(&thr[i], NULL, calc, (void*)thread_m);
        if(t != 0){
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }

    // perform semaphore synchronization among threads
    for(int i = 0; i < m; i++){
        sem_wait(&s);
    }

    printf("sum of square roots: %.6f\n", gsum);

    // clean up
    sem_destroy(&s);
    pthread_mutex_destroy(&lock);
    return 0;
}