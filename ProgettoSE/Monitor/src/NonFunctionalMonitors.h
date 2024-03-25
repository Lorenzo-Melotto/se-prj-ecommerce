#ifndef NON_FUNCTIONAL_MONITORS_H
#define NON_FUNCTIONAL_MONITORS_H

#include <stdio.h>
#include "pgsql.h"

using namespace std;

/**
* @brief Checks if the server response time for GET_FURNISHER_ID/GET_TRANSPORTER_ID/GET_FURNISHER_ID
* action is less than 46 ms
*/
void non_func_monitor_1(Con2DB log_db);

/**
* @brief Checks the transaction/seconds rate from the start of the actual simulation  
*/
void non_func_monitor_2(Con2DB log_db);

#endif