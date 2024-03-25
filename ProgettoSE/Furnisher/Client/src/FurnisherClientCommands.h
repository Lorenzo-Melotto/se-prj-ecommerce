#ifndef FURNISHER_CLIENT_COMMAND_H
#define FURNISHER_CLIENT_COMMAND_H

#include <vector>
#include <string>
#include "redisconnection.h"
#include "globals.h"
#include "random.h"
#include "pgsql.h"
#include "time_utils.h"
#include "log_to_db.h"

using namespace std;

/**
 * @brief Gets a random Furnisher Id from the server
 * 
 * @param ctx pointer to redis context
 * @param user_stream_id 
 * @param block 
 * @return int furnisher id
 */
int getFurnisherId(redisContext *ctx, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type);

/**
 * @brief Sends data of a new item to be inserted to the server
 * 
 * @param ctx pointer to redis context
 * @param furnisher_id furnisher_id of the item to be inserted
 */
void insertItem(redisContext *ctx, int furnisher_id, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type);


/**
 * @brief Sends data of a new item to be updated to the server
 * 
 * @param ctx pointer to redis context
 * @param user_stream_id 
 * @param block 
 * @param furnisher_id furnisher_id of the item to be updated
 */
void updateItem(redisContext *ctx, char *user_stream_id, int block, int furnisher_id, Con2DB ecommerce, int pid, int run_num, char *test_type);

#endif