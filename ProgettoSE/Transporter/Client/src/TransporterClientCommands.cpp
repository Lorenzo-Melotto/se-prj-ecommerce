#include "TransporterClientCommands.h"

int getTransporterId(redisContext *ctx, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type){
    if (DEBUG) printf("ACTION: GET_TRANSPORTER_ID\n");

    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %s %s %s", 
                       TRANSPORTER_STREAM_CLIENT, 
                       "GET_TRANSPORTER_ID", 
                       "",
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int transporter_id = -1;
    char trans_id[255] = {0};
    chrono::system_clock::time_point start = chrono_start();
    readServerResponse(ctx, user_stream_id, block, TRANSPORTER_STREAM_SERVER, (char *)"TRANSPORTER_ID", trans_id, NULL);
    transporter_id = atoi(trans_id);
    float duration_ms = (float)chrono_end(start) / 1000000;

    if (DEBUG) printf("Got transporter with id: %d\n", transporter_id);

    if(transporter_id < 0) return transporter_id; // no transporters in the DB

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if (DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, transporter_id, "GET_TRANSPORTER_ID");
    log_to_db_with_response_time(ecommerce, 
                                 run_num,
                                 (char *)"TRANSPORTER",
                                 date, 
                                 nanos_day, 
                                 timeadvance, 
                                 pid, 
                                 transporter_id, 
                                 (char *)"GET_TRANSPORTER_ID", 
                                 duration_ms,
                                 test_type);
    return transporter_id;
}

void takeOrder(redisContext *ctx, int transporter_id, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type){
    if (DEBUG) printf("ACTION: TAKE_ORDER\n");
    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %d %s %s", 
                       TRANSPORTER_STREAM_CLIENT, 
                       "TAKE_ORDER", 
                       transporter_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int order = -1;
    chrono::system_clock::time_point start = chrono_start();
    char order_id[255] = {0};
    readServerResponse(ctx, user_stream_id, block, TRANSPORTER_STREAM_SERVER, (char *)"ORDER_ID_TAKE", order_id, NULL);
    order = atoi(order_id);
    float duration_ms = (float)chrono_end(start) / 1000000;

    if(order < 0) return; // no orders available

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if (DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, transporter_id, "TAKE_ORDER");
    log_to_db_with_response_time(ecommerce,
                                 run_num,
                                 (char *)"TRANSPORTER",
                                 date,
                                 nanos_day,
                                 timeadvance,
                                 pid,
                                 transporter_id,
                                 (char *)"TAKE_ORDER",
                                 order,
                                 duration_ms,
                                 test_type);
}

void deliverOrder(redisContext *ctx, char *user_stream_id, int block, int transporter_id, Con2DB ecommerce, int pid, int run_num, char *test_type){
    if (DEBUG) printf("ACTION: DELIVER_ORDER\n");

    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %d %s %s", 
                       TRANSPORTER_STREAM_CLIENT, 
                       "HAS_ORDERS", 
                       transporter_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int orders_count = -1;
    chrono::system_clock::time_point start = chrono_start();
    char cnt[255] = {0};
    readServerResponse(ctx, user_stream_id, block, TRANSPORTER_STREAM_SERVER, (char *)"ORDERS_COUNT", cnt, NULL);
    orders_count = atoi(cnt);
    float duration_ms = (float)chrono_end(start) / 1000000;

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if (DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, transporter_id, "HAS_ORDERS");
    log_to_db_with_response_time(ecommerce,
                                 run_num,
                                 (char *)"TRANSPORTER",
                                 date,
                                 nanos_day,
                                 timeadvance,
                                 pid,
                                 transporter_id,
                                 (char *)"HAS_ORDERS",
                                 duration_ms,
                                 test_type);

    // se il controllo va a buon fine faccio deliver_order
    if(orders_count > 0){
        rep = RedisCommand(ctx, 
                           "XADD %s * %s %d %s %s", 
                           TRANSPORTER_STREAM_CLIENT, 
                           "DELIVER_ORDER", 
                           transporter_id,
                           "USER_STREAM_ID",
                           user_stream_id);
        assertReplyType(ctx, rep, REDIS_REPLY_STRING);
        freeReplyObject(rep);

        int order_id = -1;
        //READ ORDER_ID FROM SERVER
        chrono::system_clock::time_point start = chrono_start();
        char or_id[255] = {0};
        readServerResponse(ctx, user_stream_id, block, TRANSPORTER_STREAM_SERVER, (char *)"ORDER_ID_DELIVERY", or_id, NULL);
        order_id = atoi(or_id);
        duration_ms = (float)chrono_end(start) / 1000000;

        if(order_id < 0) return;

        nanos_day = get_day_nanos(date);
        //saves log of deliver_orders
        if (DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, transporter_id, "DELIVER_ORDER");
        log_to_db_with_response_time(ecommerce,
                                     run_num,
                                     (char *)"TRANSPORTER",
                                     date,
                                     nanos_day,
                                     timeadvance,
                                     pid,
                                     transporter_id,
                                     (char *)"DELIVER_ORDER",
                                     order_id,
                                     duration_ms,
                                     test_type);
    } else {
        if (DEBUG) printf("[INFO]: The transporter does not have any orders\n");
    }
}
