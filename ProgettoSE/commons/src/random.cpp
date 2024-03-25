#include "random.h"

unsigned int init_rng_with_time(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_t nsec = (time_t)ts.tv_nsec;
    srand(nsec);
    return nsec;
}

void init_rng(unsigned int seed){
    srand(seed);
}
                  
int rand_range(int min, int max){
    return (rand() % (max + 1 - min) + min);
}

double rand_prob(){
    return (double)rand() / (double)RAND_MAX;
}