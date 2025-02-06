#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

double gsum = 0;


int main(int argc, char *argv[]){
    printf("%s %s", argv[1], argv[2]);
    //add error checking for args

    /*
        We start with m slots in our array, we split the number of indexes we're actively working with by half each time we sum. Thr0 will be the sum of the array and then every step will be print as we iterate back through the array. The values will be replaced in place so the og values, only the sums will remain.
    
    */
   int m =  argv[1];
   //create m threads

   //sum threads
   

    //m
   

}