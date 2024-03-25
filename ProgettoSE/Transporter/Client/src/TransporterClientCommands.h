#ifndef TRANSPORTER_CLIENT_COMMAND_H
#define TRANSPORTER_CLIENT_COMMAND_H

#include "redisconnection.h"
#include "globals.h"
#include "pgsql.h"
#include "time_utils.h"
#include "log_to_db.h"


/**
 * @brief Get a random Transporter Id from the transporter's id on the server 
 * that still have some taken order to deliver
 * 
 * @param ctx pointer to redis context
 * @param user_stream_id user read group
 * @param block milliseconds to wait for server response in redis stream
 * @param ecommerce db connection
 * @param pid process id
 * @return int 
 */
int getTransporterId(redisContext *ctx, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type);

/**
 * @brief Tell the server to accept a random delivery with the transporter_id in input
 * 
 * @param ctx pointer to redis context
 * @param transporter_id transporter_id to be insert into the accepted random order
 * @param user_stream_id user read group
 * @param block milliseconds to wait for server response in redis stream
 * @param ecommerce db connection
 * @param pid process id
 */
void takeOrder(redisContext *ctx, int transporter_id, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type);

/**
 * @brief Tell the server to deliver a random order 
 * from the list of orders taken by the transporter_id in input, if the list is empty do nothing
 * 
 * @param ctx pointer to redis context
 * @param transporter_id transporter_id to be insert into the accepted random order
 * @param user_stream_id user read group
 * @param block milliseconds to wait for server response in redis stream
 * @param ecommerce db connection
 * @param pid process id
 */
void deliverOrder(redisContext *ctx, char *user_stream_id, int block, int transporter_id, Con2DB ecommerce, int pid ,int run_num, char *test_type);

#endif