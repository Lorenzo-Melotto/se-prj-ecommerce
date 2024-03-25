#include "read_args.h"

void read_args(int argc, char *argv[], int &time_to_sleep, int &run_num, char *test_type){
    if(strcmp(argv[0], "-t") == 0){
        time_to_sleep = atoi(argv[1]);
    } else {
        fprintf(stderr, "Expected -t <time_to_sleep_ms>\n");
    }

    if(strcmp(argv[2], "-r") == 0){
        run_num = atoi(argv[3]);
    } else {
        fprintf(stderr, "Expected -r <run_number>\n");
    }

    if(strcmp(argv[4], "-n") == 0){
        strncpy(test_type, argv[5], 255);
        // test_type = argv[5];
    } else {
        fprintf(stderr, "Expected -n <test_type>\n");
    }
}