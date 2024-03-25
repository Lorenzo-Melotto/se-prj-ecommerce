#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include "stdio.h"
#include "time.h"
#include "errno.h"
#include "globals.h"
#include <chrono>

using namespace std;

/**
 * @brief Pauses the process for `millisec` milliseconds
 * 
 * @param millisec Number of milliseconds to pause
 * @return int 
 */
int millisec_sleep(long millisec);


/**
 * @brief Get the nanoseconds elapsed since 1970
 * 
 * @return int long 
 */
int long get_nanoseconds();

/**
 * @brief Writes the current timestamp in YYYY-MM-DD HH:mm:ss-msms and 
 * returns the nanoseconds for more precision
 * 
 * @param buf 
 * @return long int 
 */
long int get_day_nanos(char* buf);

/**
 * @brief Updates the global simulation time
 * 
 */
void update_time();

/**
 * @brief Initializes the simulation time
 * 
 */
void init_time();

/**
 * @brief Starts a chrono clock
 * 
 */
chrono::system_clock::time_point chrono_start();

/**
 * @brief Returns nanosecond passed from chrono start in input
 * 
 */
int chrono_end(chrono::system_clock::time_point start);

#endif