#ifndef REDISCONNECTION_H
#define REDISCONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

extern "C" {
#include <hiredis/hiredis.h>
}


#define DEBUG_LOG(msg, ...)                                                   \
    do{                                                                       \
        fprintf(stderr, "%s:%d: " msg "\n", __FILE__, __LINE__, __VA_ARGS__); \
    } while(0);

#define DEBUG_ABT(msg, ...)          \
    do {                             \
        DEBUG_LOG(msg, __VA_ARGS__); \
        exit(-1);                    \
    } while(0);

#define RedisCommand(ctx, ...) \
    (redisReply *) redisCommand(ctx, __VA_ARGS__)

/**
 * @brief Checks if the server returned something. If not, aborts the program
 * 
 * @param ctx pointer to redis context
 * @param rep pointer to redis reply
 */
void assertReply(redisContext *ctx, redisReply *rep);

/**
 * @brief Checks if the server reply type is the expected one. If not,
 * aborts the program
 * 
 * @param ctx pointer to redis context
 * @param rep pointer to redis reply
 * @param type the expected type
 */
void assertReplyType(redisContext *ctx, redisReply *rep, int type);

/**
 * @brief Prints the server reply on stdout
 * 
 * @param rep pointer to redis reply
 * @param indent indentation level for printing
 */
void dumpReply(redisReply *rep, int indent);

/**
 * @brief Initializes a stream with name `streamName` with default group called 'messages'
 *
 * @param ctx pointer to redis context
 * @param streamName the name of the new stream to create
 */
void initStream(redisContext *ctx, const char *streamName);

/**
 * @brief Returns the number of streams in the reply object
 *
 * @param rep pointer to redis reply
 * @return int
 */
int getNumStreams(redisReply *rep);

/**
 * @brief Returns a pointer to the redisReply struct where the response from
 * the server is stored 
 * 
 * @param ctx pointer to redis context
 * @param group group reading
 * @param user user_id reading
 * @param block 
 * @param stream stream containing the response from the server
 * @return redisReply* 
 */
redisReply *readFromStream(redisContext *ctx, char *group, char *user, int block, const char *stream);

/**
* @brief 
*/
redisReply *readFromStartOfStream(redisContext *ctx, char *group, char *user, int block, const char *stream);

/**
* @brief Checks if the server response contains the response for the user_stream_id in input:
* if it does extracts the data and saves it into response1 and response2 (if they are not null)
*   and ack the message; 
* if it doesn't Claims the message for the right user_stream_id ;
* 
* @param ctx pointer to redis context
* @param user_stream_id client id reading the server response
* @param block
* @param stream stream with the server response to be read
* @param command command whose response the client wants to read
* @param response2 where the data in position 3 gets copied
*/
void readServerResponse(redisContext *ctx, char *user_stream_id, int block, const char *stream, char *command, char *response1, char *response2);

/**
 * @brief Copies the stream name inside `streamName`
 *
 * @param rep pointer to redis reply
 * @param streamName string where the stream name is copied
 * @param idx index of the stream
 */
void readStreamName(redisReply *rep, char *streamName, long unsigned int idx);

/**
 * @brief Returns the number of elements in the `idx` stream
 *
 * @param rep pointer to redis reply
 * @param idx index of the stream
 * @return int
 */
int getStreamLen(redisReply *rep, long unsigned int idx);
/**
 * @brief Reads the id of the `msgIdx` in the `streamIdx` stream
 *
 * @param rep pointer to redis reply
 * @param streamIdx index of the stream
 * @param msgIdx index of the element in the stream
 * @param msgId where the id is copied
 */
void readStreamMessageId(redisReply *rep, long unsigned int streamIdx, int msgIdx, char *msgId);

/**
 * @brief Returns the value of the `msgIdx` in the `streamIdx` stream
 *
 * @param rep pointer to redis reply
 * @param streamIdx the index of the stream
 * @param msgIdx the index of the element in the strem
 */
int getStreamNumOfMessages(redisReply *rep, long unsigned int streamIdx, int msgIdx);

/**
 * @brief Reads the "value" of the `msgIdx` message in the `streamIdx` stream
 *
 * @param rep pointer to redis reply
 * @param streamIdx the index of the stream
 * @param msgIdx the index of the message in the stream
 * @param entry the entry number in the message
 * @param value where the "key", "value" pair gets copied
 */
void getStreamMessagesValues(redisReply *rep, long unsigned int streamIdx, int msgIdx, int entry, char *value);
/**
 * @brief Reads the key/value entry of the `msgIdx` message in the `streamIdx` stream
 *
 * @param rep pointer to redis reply
 * @param streamIdx the index of the stream
 * @param msgIdx the index of the message in the stream
 * @param key where the key of the message gets copied
 * @param value where the value of the message gets copied
 */
void getStreamMessageKeyValue(redisReply *rep, long unsigned int streamIdx, int msgIdx, char *key, char *value);

/**
* @brief ACK the message with msg_id into the stream 
*
*@param stream 
*@param msg_id 
*/
void ackMessage(redisContext *ctx, const char* stream, char *msg_id);
// redisReply *getPendingMessages(redisContext *ctx, const char *stream, char *group);

#endif
