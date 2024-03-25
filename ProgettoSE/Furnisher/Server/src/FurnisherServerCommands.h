#ifndef FURNISHER_SERVER_COMMANDS_H
#define FURNISHER_SERVER_COMMANDS_H

#include <chrono>
#include "redisconnection.h"
#include "pgsql.h"
#include "globals.h"
#include "time_utils.h"

using namespace std;

/**
 * @brief Inserts the new item data received from client into the DB
 * 
 * @param rep pointer to redis reply  with
 *                       FURNISHER_STREAM_CLIENT,
                        "INSERT_ITEM",
                        item_id,
                        "QUANTITY",
                        quantity,
                        "FURNISHER",
                        furnisher_id
 * @param ctx pointer to redis context
 * @param ecommerce db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void insertItem(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage);

/**
 * @brief Update the item on the db with the quantity received 
 * 
 * @param rep pointer to redis reply with
 *                       FURNISHER_STREAM_CLIENT,
                        "UPDATE_ITEM",
                        item,
                        "QUANTITY",
                        new_quantity,
                        "FURNISHER_ID",
                        furnisher_id
 * @param ctx pointer to redis context
 * @param ecommerce db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void updateItem(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage);

/**
 * @brief Return to the furnisher client a random Item id with the furnisher_id in *rep
 * 
 * @param ctx pointer to redis context
 * @param rep pointer to redis reply with FURNISHER_STREAM_CLIENT, "LIST_OF_ITEMS", furnisher_id
 * @param ecommerce db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void listOfItems(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage);

/**
 * @brief Return to the furnisher client a random furnisher id
 * 
 * @param ctx pointer to redis context
 * @param rep pointer to redis reply 
 * @param ecommerce db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void getFurnisherId(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage);

#endif