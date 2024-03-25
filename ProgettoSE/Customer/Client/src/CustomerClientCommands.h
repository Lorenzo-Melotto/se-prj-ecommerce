#ifndef CUSTOMER_CLIENT_COMMANDS_H
#define CUSTOMER_CLIENT_COMMANDS_H

#include "redisconnection.h"
#include "globals.h"
#include "random.h"
#include "pgsql.h"
#include "time_utils.h"
#include "log_to_db.h"

/**
 * @brief Gets a random customer Id from the server or -1 if no customer already exists
 * 
 * @param ctx pointer to redis context
 * @param user_stream_id 
 * @param block 
 * @return int customer id
 */
int getCustomerId(redisContext *ctx, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type);

/**
 * @brief Gets a random Item id from the server and sends the server a buy request with 
 *                              CUSTOMER_STREAM_CLIENT,
                                "BUY_ITEM", 
                                item_id,
                                "QUANTITY",
                                item_quantity,
                                "CUSTOMER_ID",
                                customer_id);
 * 
 * @param ctx pointer to redis context
 * @param user_stream_id 
 * @param block 
 * @param customer_id id of the customer sending the request
 */
 void buyItem(redisContext *ctx, char *user_stream_id, int block, int customer_id, Con2DB ecommerce, int pid, int run_num, char *test_type);

#endif