#ifndef READ_ARGS_H
#define READ_ARGS_H

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Reads command line arguments looking for the `-t` and `-r` args. The
 * argument `-r` is required for the execution
 * 
 * @param argc Indicates the number of arguments in `argv`
 * @param argv Array of strings that contains the arguments
 * @param time_to_sleep Time to sleep betwen each action
 * @param run_num The run number used in the logs
 */
void read_args(int argc, char *argv[], int &time_to_sleep, int &run_num, char *test_type);

#endif