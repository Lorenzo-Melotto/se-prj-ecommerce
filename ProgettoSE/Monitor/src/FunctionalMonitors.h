#ifndef FUNCTIONAL_MONITORS_H
#define FUNCTIONAL_MONITORS_H

#include <stdio.h>
#include "pgsql.h"

using namespace std;

/**
* @brief Checks if a customer buys more items than available 
*/
void func_monitor_1(Con2DB log_db);
/**
* @brief  Checks if time passed between started_delivery and delivery_time is less than 7 simulated days (200 s)
*/
void func_monitor_2(Con2DB log_db);
/**
* @brief Check if a customer really bought a certain product
*/
void func_monitor_3(Con2DB log_db);

#endif