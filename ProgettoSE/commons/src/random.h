#ifndef RANDOM_H
#define RANDOM_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/**
 * @brief Initializes the random number generator using the current time
 * 
 */
unsigned int init_rng_with_time();

/**
 * @brief Initializes the random number generator with the given seed
 * 
 * @param seed 
 */
void init_rng(unsigned int seed);

/**
 * @brief Generates a random number in the range (`min`, `max`) with both ends
 * included
 * 
 * @param min 
 * @param max 
 * @return int 
 */
int rand_range(int min, int max);

/**
 * @brief Generates a random number in the range (0,1)
 * 
 * @return double 
 */
double rand_prob();

#endif