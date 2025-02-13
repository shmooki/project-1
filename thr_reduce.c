#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>

/******************************************************************************
 * Program Name: thr_reduce
 * Authors: Ray Feingold 2804053, Matthew Kovach 2824267
 * Course: CIS345 Section 50
 * Date: 02/14/2025
 * Description: Utilize parallel processing and thread redcution among 'm' 
 * number of threads to calculate the sum of square roots from 1 to 'n'
 *****************************************************************************/

/**
 * initialize global variables of square root sum array, pthread array, 
 * 'm' number of threads var, and 'r' amount of reductions var
 */
double* gsum_array;
pthread_t *thr;
int m, r;

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
} pthread_sqrt_t;

/**
 * @brief Computes the sum of square roots from the lower to upper bound of a pthread_sqrt_t
 * This function is called in main() during a pthread_create call. A thread is either primary
 * (upper half of thr[m] array or secondary (upper half of thr[m] array). After a thread 
 * computes its partial sum, a primary thread will join with its secondary thread after it 
 * has terminated. This process is used to achieve parallel thread reduction by splitting 
 * the square root sum calculations among multiple threads for complete parallel processing.
 * @param thread_args The pthread_sqrt_t passed during a pthread_create call.
 * @return Returns nothing. Exits thread at end of function.
 */
void *calc(void *thread_args){
    // create local pthread_sqrt_t variable received from pthread_create args
    pthread_sqrt_t *cur_thread = (pthread_sqrt_t*)thread_args;

     // initialize local thread partial square root sum
    double lsum = 0;

    // for the range of indeces of 'primary' threads
    int active_threads = m;

    // calculate sum of square roots from the upper and lower bounds of struct pthread_sqrt_t
    for(int i = cur_thread->l_bound; i <= cur_thread->u_bound; i++){
        lsum += sqrt(i);
    }

    // add partial sum to gsum
    gsum_array[cur_thread->t_id] = lsum;
    
    /**
     * To achieve parallel thread reduction, we must identify what threads are 'primary'
     * or 'secondary'. We do this by splitting the total number of threads in half where
     * threads 0 -> ((m / 2) - 1) are considered 'primary' and thus continue executing. Similarly,
     * threads m / 2 -> m are considered 'secondary' and exit after computing its partial sum.
     * Upon calculating its partial sum, a primary thread will join with a partner thread that is 
     * m / 2 distance away from it and combine sums. The total number of active threads is then
     * halved before continuing for 'r' number of reductions, where r = log(m)
     */
    int partner_thread_id;
    for(int i = 1; i <= r; i++){
        if(cur_thread->t_id < active_threads / 2) {
            partner_thread_id = cur_thread->t_id ^ (1 << (r - i));
            pthread_join(thr[partner_thread_id], NULL);
            printf("\nthread[%d] waiting for partner thread[%d]", cur_thread->t_id, partner_thread_id);
            gsum_array[cur_thread->t_id] += gsum_array[partner_thread_id];
        } 
        else{ 
            break;
        }

        active_threads /= 2;
    }

    if (cur_thread->t_id == 0) {
        for (int i = 0; i < m; i++) {
            printf("\nthr[%d] sum: %.6f", i, gsum_array[i]);
        }

        printf("\nthread[0] exiting with a final sum of %.6f. . ."
                "\nProgram has completed."
                "\nExiting. . .\n", gsum_array[0]);

        // clean up
        free(cur_thread);
        pthread_exit(NULL);
    }

    printf("\nthread[%d] exiting . . .", cur_thread->t_id);

    // clean up
    free(cur_thread);
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

    /**
     * compute r = log2(m) by bit shifting a temp var 'i' that is equal to 'm'
     * this essentially 'counts' the number of zeroes in the binary value of 'm'
     */
    int i = m;
    while(i > 1){
        i >>= 1;
        r++;
    }

    /**
     * create 'm' number of threads to calculate partial square root sums
     * between the determined lower to upper bound integer range
     * of current cur_thread struct
     */
    thr = (pthread_t *)malloc(m * sizeof(pthread_t));
    gsum_array = malloc(m * sizeof(double));
    for(int i = m - 1; i >= 0; i--){
        pthread_sqrt_t *cur_thread = malloc(sizeof(pthread_sqrt_t));
        cur_thread->t_id = i;
        cur_thread->l_bound = i * (n / m) + 1;
        cur_thread->u_bound = (i + 1) * (n / m);
        
        // check if successful thread creation
        if (pthread_create(&thr[i], NULL, calc, (void*)cur_thread) != 0) {
            printf("\nFailed to create thread\nExiting. . .\n");
            return 1;
        }
    }

    // exit main thread without terminating active threads
    pthread_exit(NULL);
}