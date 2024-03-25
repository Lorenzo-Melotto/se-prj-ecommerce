#ifndef TRANSPORTER_SERVER_COMMANDS_H
#define TRANSPORTER_SERVER_COMMANDS_H

#include "redisconnection.h"
#include "pgsql.h"
#include "globals.h"
#include "time_utils.h"
#include "random.h"

/**
 * @brief Updates the started_delivery and transporter_id of a random Order still to be taken 
 * with current time and transporter_id in rep
 * 
 * @param rep pointer to redis reply  with
 *                      

 * @param db db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void takeOrder(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);

/**
 * @brief Updates the delivery_time of a random Order accepted by the furnisher_id in rep 
 * 
 * @param rep pointer to redis reply with
 *         
 * @param ctx pointer to redis context                
 * @param db db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void deliverOrder(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);

/**
 * @brief Returns number of orders taken from the transporter_id in rep
 * @param rep pointer to redis reply with
 *         
 * @param ctx pointer to redis context                
 * @param db db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
*/
void hasOrders(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);

/**
 * @brief Return to the transporter client a random transporter id
 * 
 * @param rep pointer to redis reply with
 * 
 * @param ctx pointer to redis context                      
 * @param db db connection
 * @param numStream stream index
 * @param numMessage message index in the stream
 */
void getTransporterId(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage);


#endif