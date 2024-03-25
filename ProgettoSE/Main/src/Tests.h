#ifndef TESTS_H
#define TESTS_H

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include <errno.h>
#include "random.h"
#include "globals.h"
#include "redisconnection.h"
#include "time_utils.h"
#include "random.h"

using namespace std;


/**
 * @brief Spawns `client_num` client processes of each actor and `server_num` server processes for each actor
 * 
 * @param ctx Redis context for deleting and creating streams
 * @param run_num The run number
 * @param tes_num The number of the test
 * @param time_to_wait The time to wait between each action
 * @param furn_client_num The number of furnisher clients to spawn
 * @param furn_server_num The number of furnisher servers to spawn
 * @param cust_client_num The number of customer clients to spawn
 * @param cust_server_num The number of customer servers to spawn
 * @param tran_client_num The number of transporter clients to spawn
 * @param tran_server_num The number of transporter servers to spawn
 * @param test_name The name of the test
 */
// void test(redisContext *ctx, int run_num, int client_num, int server_num, char *test_name);
void test(redisContext *ctx, int run_num, int test_num, int time_to_wait, 
          int furn_client_num, int furn_server_num, int cust_client_num, 
          int cust_server_num, int tran_client_num, int tran_server_num, 
          char *test_name);

void runMonitors();

#endif