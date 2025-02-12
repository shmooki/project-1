#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>

/******************************************************************************
 * Program Name: thr_atomic
 * Authors: Ray Feingold 2804053, Matthew Kovach
 * Course: CIS345 Section 50
 * Date: 02/14/2025
 * Description: Utilize parallel processing among 'm' number of threads to calculate
 * the sum of square roots from 1 to 'n'
 *****************************************************************************/

double* gsum_array;
pthread_t *thr;
int m, r, bound;
sem_t s;

/**
 * @brief This struct is used by a thread to store the integer range
 * from l_bound -> u_bound of square root sums to calculate
 * @struct pthread_sqrt_t
 * @param t_id thread id
 * @param l_bound integer range lower bound
 * @param u_bound integer range upper bound
 * @param p_sum partial square root sum
 */
typedef struct{
    int t_id; /**< pthread thread number */
    int l_bound; /**< square root sum lower bound */
    int u_bound; /**< square root sum upper bound */
    double p_sum; /**< square root partial sum from l_bound -> u_bound */
} pthread_sqrt_t;

/**
 * @brief Computes the sum of square roots from the lower to upper bound of a pthread_sqrt_t
 * @param thread_args The pthread_sqrt_t passed during a pthread_create call
 */
void *calc(void *thread_args){
    // create local pthread_sqrt_t variable received from pthread_create args
    pthread_sqrt_t *cur_range = (pthread_sqrt_t*)thread_args;
    cur_range->p_sum = 0;

    // calculate the range of indeces of 'primary' threads
    int active_thread = m / 2;

    //printf("\ninside thread %d\n", cur_range->t_id);
    // calculate sum of square roots from the upper and lower bounds of struct pthread_sqrt_t
    for(int i = cur_range->l_bound; i <= cur_range->u_bound; i++){
        cur_range->p_sum += sqrt(i);
    }

    // add partial sum to gsum
    gsum_array[cur_range->t_id] = cur_range->p_sum;
    
    // if(cur_range->t_id % (1 << i) == 0){
    // perform parallel reduction
    for(int i = 0; i < r; i++){
        // printf("\n%.6f is the sum of thread %d\n", cur_range->p_sum, cur_range->t_id);
        int partner_thread_id;
        if(cur_range->t_id < active_thread){
            partner_thread_id = cur_range->t_id ^ (1 << (r - i - 1));
            pthread_join(thr[partner_thread_id], NULL);
            printf("\nI'm thread %d, my partner is thread %d\n", cur_range->t_id,partner_thread_id);
            if(cur_range->t_id < partner_thread_id){
                gsum_array[cur_range->t_id] += gsum_array[partner_thread_id];
                m /= 2;
            }
            else{
                break;
            }
            
            printf("\nOur sum is %.6f\n", gsum_array[cur_range->t_id]);
        // printf("\npsum in gsum in thread %d is %.6f\n", cur_range->t_id, gsum_array[cur_range->t_id]);
        } 
        else{ 
            break;
        }

        // reduce threads by half
       
    }
    
    // if(cur_range->t_id == 0){
    //     printf("\n length is %d", bound);
    //     for(int i = 0; i < bound; i++){
    //         printf("\n Thread [%d]: %.6f\n",i,gsum_array[i]);
    //     }
        
    // }
    // clean up
    free(cur_range);
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    // check if m & n are provided in command-line arguments
    if(argc != 3){
        printf("\nIncorrect number of command-line arguments provided\n");
        return 1;
    }

    // from command-line args-> 'm' = number of threads, 'n' = integer range upper bound
    m = atoi(argv[1]);
    int n = atoi(argv[2]);

    // compute r = log2(num_threads)
    //this could be a while loop
    for(int i = m ; i > 1; i >>= 1){
        r++;
    }
    /**
     * create 'm' number of threads to calculate partial square root sums
     * between the determined lower to upper bound integer range
     * of current cur_range
     */
    bound = m;
    thr = (pthread_t *)malloc(m * sizeof(pthread_t));
    gsum_array = malloc(m * sizeof(double));
    for(int i = bound - 1; i >= 0; i--){
        pthread_sqrt_t *cur_range = malloc(sizeof(pthread_sqrt_t));
        cur_range->t_id = i;
        cur_range->l_bound = i * (n / bound) + 1;
        cur_range->u_bound = (i + 1) * (n / bound);
        
        // check if successful thread creation
        if (pthread_create(&thr[i], NULL, calc, (void*)cur_range) != 0) {
            printf("\nFailed to create thread");
            return 1;
        }
    }

    //print shared global net square root sum
    printf("\nsum of square roots: %.6f\n", gsum_array[0]);

    for(int i = 0; i < bound; i++){
        printf("\nsum in thr[%d] : %.6f\n", i, gsum_array[i]);
    }
    free(thr);
    free(gsum_array);
    return 0;
}