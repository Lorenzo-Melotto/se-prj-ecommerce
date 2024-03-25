#ifndef CUSTOMER_SERVER_COMMANDS_H
#define CUSTOMER_SERVER_COMMANDS_H

#include "redisconnection.h"
#include "pgsql.h"
#include "globals.h"

/**
 * @brief Inserts the new order data into the db tables Orders and Item-order
 * 
 * @param ctx pointer to redis context
 * @param rep pointer to the redis reply with CUSTOMER_STREAM_CLIENT,"BUY_ITEM", item_id,"QUANTITY",item_quantity,"CUSTOMER_ID",customer_id
 * @param db db connection
 * @param numStream  stream index 
 * @param numMessage message index in the stream
 */
void buyItem(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);

/**
 * @brief Returns to the client Id and Available Quantity of a random Item from the db
 * 
 * @param ctx pointer to redis context
 * @param rep pointer to redis reply with CUSTOMER_STREAM_CLIENT, "FURNISHER_ID", furnisher id
 * @param db db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void listOfItems(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);

/**
 * @brief Returns to the client a random Customer Id or -1 if no customer already exists
 * 
 * @param ctx pointer to redis context
 * @param db db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void getCustomerId(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);

#endif