#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

double gsum = 0;

//argv[1] = m, argv[2] = n
int main(int argc, char *argv[]){
    printf("%s %s", argv[1], argv[2]);
}