#include "redisconnection.h"

void assertReply(redisContext *ctx, redisReply *rep){
    if(rep == NULL) {
        DEBUG_ABT("Redis reply is NULL. Error: %s", ctx->errstr);
    }
}

void assertReplyType(redisContext *ctx, redisReply *rep, int type){
    assertReply(ctx, rep); // checks if the reply is not null
    if (rep->type != type) {
        DEBUG_ABT("Redis reply type is %d but %d was expected", rep->type, type); 
    }
}

void dumpReply(redisReply *rep, int indent){
    sds buffer = sdsempty();

    // deal with each reply type
    switch(rep->type) {
        case REDIS_REPLY_STRING:
            buffer = sdscatfmt(buffer, "(string) %s\n", rep->str);
            break;
        case REDIS_REPLY_STATUS:
            buffer = sdscatfmt(buffer, "(status) %s\n", rep->str);
            break;
        case REDIS_REPLY_INTEGER:
            buffer = sdscatlen(buffer, "(integer) %lld\n", rep->integer);
            break;
        case REDIS_REPLY_NIL:
            buffer = sdscatlen(buffer, "(nil)\n", 6);
            break;
        case REDIS_REPLY_ERROR:
            buffer = sdscatfmt(buffer, "(error) %s\n", rep->str);
            break;
        case REDIS_REPLY_ARRAY:
            // recursively deal with each element of the array
            for(size_t i = 0; i < rep->elements; i++) {
                dumpReply(rep->element[i], indent + 2);
            }
            break;
        default:
            DEBUG_ABT("Don't know how to deal with reply type %d", rep->type);
    }

    if (sdslen(buffer) > 0) {
        // add indentation
        for(size_t i = 0; i < (size_t)indent; i++){
            printf(" ");
        }
        printf("%s", buffer);
    }

    sdsfree(buffer);
}

void initStream(redisContext *ctx, const char *streamName) {
    redisReply *rep = RedisCommand(ctx, "XGROUP CREATE %s messages $ MKSTREAM", streamName);
    assertReply(ctx, rep);
    freeReplyObject(rep);
}

#if 0
/* This is the reply object returned by redisCommand() */
typedef struct redisReply {
    int type; /* REDIS_REPLY_* */
    long long integer; /* The integer when type is REDIS_REPLY_INTEGER */
    size_t len; /* Length of string */
    char *str; /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
    size_t elements; /* number of elements, for REDIS_REPLY_ARRAY */
    struct redisReply **element; /* elements vector for REDIS_REPLY_ARRAY */
} redisReply;

// element is an array of redisReply, so each time you do rep->element[idx]
// you get access to all the redisReply fields
#endif


redisReply *readFromStream(redisContext *ctx, char *group, char *user, int block, const char *stream) {
    redisReply *rep = RedisCommand(ctx, "XREADGROUP GROUP %s %s BLOCK %d STREAMS %s >",
                                   group, user, block, stream);
    assertReply(ctx, rep);
    return rep;
}

redisReply *readFromStartOfStream(redisContext *ctx, char *group, char *user, int block, const char *stream) {
    redisReply *rep = RedisCommand(ctx, "XREADGROUP GROUP %s %s BLOCK %d STREAMS %s 0-0",
                                   group, user, block, stream);
    assertReply(ctx, rep);
    return rep;
}

void readServerResponse(redisContext *ctx, 
                        char *user_stream_id, 
                        int block, 
                        const char *stream, 
                        char *command,
                        char *response1, 
                        char *response2){
    redisReply *rep;
    bool got_my_message = false;
    bool is_first_read = true;
    while(!got_my_message){
        if(is_first_read){
            rep = readFromStream(ctx, (char *)"messages", user_stream_id, block, stream);
            // dumpReply(rep, 0);
            is_first_read = !is_first_read;
        } else {
            rep = readFromStartOfStream(ctx, (char *)"messages", user_stream_id, block, stream);
            // dumpReply(rep, 0);
        }
        for(int stream_num = 0; stream_num < getNumStreams(rep); stream_num++){
            for(int msg_num = 0; msg_num < getStreamLen(rep, stream_num); msg_num++){
                char msg_id[255] = {0};
                readStreamMessageId(rep, stream_num, msg_num, msg_id);
                char client_id[255] = {0};
                getStreamMessagesValues(rep, stream_num, msg_num, 5, client_id);
                if(strcmp(user_stream_id, client_id) == 0){
                    // printf("The ids match! %s = %s\n", user_stream_id, client_id);
                    char key[255] = {0};
                    getStreamMessagesValues(rep, stream_num, msg_num, 0, key);
                    // printf("Key in server response: %s\n", key);
                    // il messaggio è rivolto a noi: processarlo e poi XACK
                    if(strcmp(key, command) == 0){
                        if(response1 != NULL) getStreamMessagesValues(rep, stream_num, msg_num, 1, response1);
                        if(response2 != NULL) getStreamMessagesValues(rep, stream_num, msg_num, 3, response2);
                        ackMessage(ctx, stream, msg_id);
                        got_my_message = true;
                    } /* else {
                        printf("Received an unexpected command: expected->%s, got->%s\n", command, key);
                    } */
                } else {
                    //claim the message for the right client_id
                    printf("The ids DO NOT MATCH! %s != %s\n", user_stream_id, client_id);
                    redisReply *claim_rep = RedisCommand(ctx, 
                                                         "XCLAIM %s %s %s 0 %s",
                                                         stream,
                                                         (char *)"messages",
                                                         client_id,
                                                         msg_id);
                    freeReplyObject(claim_rep);
                    break;
                }
            }
        }
    }
    freeReplyObject(rep);
}

void ackMessage(redisContext *ctx, const char* stream, char *msg_id){
    // printf("ACK message with id %s from stream %s\n", msg_id, stream);
    redisReply *rep = RedisCommand(ctx, "XACK %s messages %s", stream, msg_id);
    assertReplyType(ctx, rep, REDIS_REPLY_INTEGER);
    freeReplyObject(rep);
}

int getNumStreams(redisReply *rep) {
    return rep->elements;
}

void readStreamName(redisReply *rep, char *streamName, long unsigned int idx) {
    strcpy(streamName, rep->element[idx]->element[0]->str);
}

int getStreamLen(redisReply *rep, long unsigned int idx) {
    return rep->element[idx]->element[1]->elements;
}

void readStreamMessageId(redisReply *rep, long unsigned int streamIdx, int msgIdx, char *msgId) {
    strcpy(msgId, rep->element[streamIdx]->element[1]->element[msgIdx]->element[0]->str);
}

int getStreamNumOfMessages(redisReply *rep, long unsigned int streamIdx, int msgIdx) {
    return rep->element[streamIdx]->element[1]->element[msgIdx]->element[1]->elements;
}

void getStreamMessagesValues(redisReply *rep, long unsigned int streamIdx, int msgIdx, int entry, char *value) {
    strcpy(value, rep->element[streamIdx]->element[1]->element[msgIdx]->element[1]->element[entry]->str);
}

void getStreamMessageKeyValue(redisReply *rep, long unsigned int streamIdx, int msgIdx, char *key, char *value) {
    strcpy(key, rep->element[streamIdx]->element[1]->element[msgIdx]->element[1]->element[0]->str); // key
    strcpy(value, rep->element[streamIdx]->element[1]->element[msgIdx]->element[1]->element[1]->str); // value
}
