#include "FurnisherClientCommands.h"

int getFurnisherId(redisContext *ctx, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type) {
    if(DEBUG) printf("ACTION: GET_FURNISHER_ID\n");

    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %s %s %s", 
                       FURNISHER_STREAM_CLIENT, 
                       "GET_FURNISHER_ID", 
                       "",
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int furnisher_id = -1;
    char furn_id[255] = {0};

    chrono::system_clock::time_point start = chrono_start();
    readServerResponse(ctx, user_stream_id, block, FURNISHER_STREAM_SERVER, (char *)"GOT_FURNISHER_ID",furn_id, NULL);
    furnisher_id = atoi(furn_id);
    float duration_ms = (float)chrono_end(start) / 1000000;

    if(DEBUG) printf("Existing furnisher with id got: %d\n", furnisher_id);
    if(furnisher_id < 0) return furnisher_id;

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if(DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, furnisher_id, "GET_FURNISHER_ID");
    log_to_db_with_response_time(ecommerce, 
                                 run_num,
                                 (char *)"FURNISHER", 
                                 date, 
                                 nanos_day, 
                                 timeadvance, 
                                 pid, 
                                 furnisher_id, 
                                 (char *)"GET_FURNISHER_ID", 
                                 duration_ms,
                                 test_type);
    return furnisher_id;
}

void insertItem(redisContext *ctx, int furnisher_id, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type) {
    if(DEBUG) printf("ACTION: INSERT from furnisher %d\n", furnisher_id);

    redisReply *rep;
    int item_id = rand();
    int item_quantity = rand_range(1, 100);

    if(DEBUG) printf("Inserting item with id %d from furnisher %d\n", item_id, furnisher_id);
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %d %s %d %s %d %s %s", 
                       FURNISHER_STREAM_CLIENT,
                       "INSERT_ITEM", 
                       item_id,
                       "QUANTITY",
                       item_quantity,
                       "FURNISHER_ID",
                       furnisher_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    chrono::system_clock::time_point start = chrono_start();
    readServerResponse(ctx, user_stream_id, block, FURNISHER_STREAM_SERVER, (char *)"OK_INSERT_ITEM", NULL, NULL);
    float duration_ms = (float)chrono_end(start) / 1000000;

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if(DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, furnisher_id, "INSERT_ITEM");
    log_to_db_with_response_time(ecommerce, 
                                 run_num,
                                 (char *)"FURNISHER", 
                                 date, 
                                 nanos_day, 
                                 timeadvance, 
                                 pid, 
                                 furnisher_id, 
                                 (char *)"INSERT_ITEM", 
                                 item_id,
                                 duration_ms,
                                 test_type);
}

void updateItem(redisContext *ctx, char *user_stream_id, int block, int furnisher_id, Con2DB ecommerce, int pid, int run_num, char *test_type) {
    if(DEBUG) printf("ACTION: UPDATE from furnisher %d\n", furnisher_id);

    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %d %s %s", 
                       FURNISHER_STREAM_CLIENT, 
                       "LIST_OF_ITEMS", 
                       furnisher_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int item = -1;
    int qta = -1;
    char item_id[255] = {0};
    char item_qta[255] = {0};
    chrono::system_clock::time_point start = chrono_start();
    readServerResponse(ctx, user_stream_id, block, FURNISHER_STREAM_SERVER, (char *)"ITEM_ID", item_id, item_qta);
    item = atoi(item_id);
    qta = atoi(item_qta);
    float duration_ms = (float)chrono_end(start) / 1000000;

    if(DEBUG) printf("Item: %d, quantity: %d\n", item, qta);
    if(item == -1 || qta == -1) return; // if there are no items, skip the update

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if(DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, furnisher_id, "LIST_OF_ITEMS");
    log_to_db_with_response_time(ecommerce, 
                                 run_num,
                                 (char *)"FURNISHER", 
                                 date, 
                                 nanos_day, 
                                 timeadvance, 
                                 pid, 
                                 furnisher_id, 
                                 (char *)"LIST_OF_ITEMS", 
                                 item, 
                                 duration_ms,
                                 test_type);

    int new_quantity = rand_range(qta, 100);
    if(DEBUG) printf("Furnisher with ID %d wants to update item %d with new quantity %d\n", furnisher_id, item, new_quantity);
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %d %s %d %s %d %s %s", 
                       FURNISHER_STREAM_CLIENT,
                       "UPDATE_ITEM",
                       item,
                       "QUANTITY",
                       new_quantity,
                       "FURNISHER_ID",
                       furnisher_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    chrono::system_clock::time_point start1 = chrono_start();
    readServerResponse(ctx, user_stream_id, block, FURNISHER_STREAM_SERVER, (char *)"OK_UPDATE_ITEM", NULL, NULL);
    duration_ms = (float)chrono_end(start1) / 1000000;

    nanos_day = get_day_nanos(date);
    if(DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, furnisher_id, "UPDATE_ITEM");
    log_to_db_with_response_time(ecommerce,
                                 run_num, 
                                 (char *)"FURNISHER", 
                                 date, 
                                 nanos_day, 
                                 timeadvance, 
                                 pid, 
                                 furnisher_id, 
                                 (char *)"UPDATE_ITEM", 
                                 item, 
                                 duration_ms,
                                 test_type);
}