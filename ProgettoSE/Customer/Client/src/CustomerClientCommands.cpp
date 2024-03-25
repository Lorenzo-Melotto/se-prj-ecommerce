#include "CustomerClientCommands.h"

void buyItem(redisContext *ctx, char *user_stream_id, int block, int customer_id, Con2DB ecommerce, int pid, int run_num, char *test_type){
    if(DEBUG) printf("ACTION: BUY_ITEM\n");
    //get the list of available items from the server (gets just a random id)
    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %s %s %d %s %s", 
                       CUSTOMER_STREAM_CLIENT, 
                       "LIST_OF_ITEMS", 
                       "",
                       "CUSTOMER_ID",
                       customer_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int item_id = -1;            
    int available_quantity = -1; // quantity to read from the server

    char it_id[255] = {0};
    char quant[255] = {0};
    chrono::system_clock::time_point start = chrono_start();
    readServerResponse(ctx, user_stream_id, block, CUSTOMER_STREAM_SERVER, (char *)"ITEM_ID", it_id, quant);
    item_id = atoi(it_id);
    available_quantity = atoi(quant);
    float duration_ms = (float)chrono_end(start) / 1000000;

    //Saves log of the "LIST_OF_ITEMS"
    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if(DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, customer_id, "LIST_OF_ITEMS");
    log_to_db_with_response_time(ecommerce,
                                 run_num,
                                (char *)"CUSTOMER",
                                 date,
                                 nanos_day,
                                 timeadvance,
                                 pid,
                                 customer_id,
                                 (char *)"LIST_OF_ITEMS",
                                 duration_ms,
                                 test_type);

    if(available_quantity <= 1) return;

    int item_quantity = rand_range(1, get_min(5, available_quantity));
    if(DEBUG) printf("Customer %d tries to buy item %d with quantity %d\n", customer_id, item_id, item_quantity);

    // send the buy request to the server
    // Message: BUY_ITEM item_id QUANTITY quantity CUSTOMER_ID customer_id
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %d %s %d %s %d %s %s", 
                       CUSTOMER_STREAM_CLIENT,
                       "BUY_ITEM", 
                       item_id,
                       "QUANTITY",
                       item_quantity,
                       "CUSTOMER_ID",
                       customer_id,
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    PGresult *res;
    char sqlcmd[1000] = {0};
    nanos_day = get_day_nanos(date);
    if (DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, customer_id, "BUY_ITEM");

    //reads order_id from server 
    int order_id = -1; 
    char or_id[255] = {0};
    chrono::system_clock::time_point start1 = chrono_start();
    readServerResponse(ctx, user_stream_id, block, CUSTOMER_STREAM_SERVER, (char *)"ORDER_ID", or_id, NULL);
    order_id = atoi(or_id);
    float duration_ms1 = (float)chrono_end(start1) / 1000000;

    sprintf(sqlcmd, "BEGIN");
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);
    sprintf(sqlcmd, 
            "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, customer_id, actor_action, order_id, response_time_ms, test_type) "
            "VALUES (%d, '%s', %ld, %d, %d, '%s', %d, %f, '%s') ON CONFLICT DO NOTHING;",
            run_num,
            date, 
            nanos_day, 
            pid, 
            customer_id, 
            "BUY_ITEM", 
            order_id,
            duration_ms1,
            test_type);
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    //inserts history of new quantity
    int new_quantity = available_quantity - item_quantity;
    sprintf(sqlcmd, 
            "INSERT INTO item_quantity_history (order_id, item_id, new_quantity) "
            "VALUES (%d, %d, %d);",
            order_id, 
            item_id,
            new_quantity);
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res); 

    sprintf(sqlcmd, "COMMIT");
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);
}

int getCustomerId(redisContext *ctx, char *user_stream_id, int block, Con2DB ecommerce, int pid, int run_num, char *test_type) {
    if(DEBUG) printf("ACTION: GET_CUSTOMER_ID\n");

    redisReply *rep;
    rep = RedisCommand(ctx, 
                       "XADD %s * %s %s %s %s", 
                       CUSTOMER_STREAM_CLIENT, 
                       "GET_CUSTOMER_ID", 
                       "",
                       "USER_STREAM_ID",
                       user_stream_id);
    assertReplyType(ctx, rep, REDIS_REPLY_STRING);
    freeReplyObject(rep);

    int customer_id = -1;
    char cust_id[255]= {0};
    chrono::system_clock::time_point start = chrono_start();
    readServerResponse(ctx, user_stream_id, block, CUSTOMER_STREAM_SERVER, (char *)"CUSTOMER_ID", cust_id, NULL);
    customer_id = atoi(cust_id);
    float duration_ms = (float)chrono_end(start) / 1000000;
    if(DEBUG) printf("Got customer with id: %d\n", customer_id);

    if(customer_id < 0) return customer_id;

    char date[255] = {0};
    nanos_day = get_day_nanos(date);
    if(DEBUG) printf("%s, %ld, %f, %d, %d, %s\n", date, nanos_day, timeadvance, pid, customer_id, "GET_CUSTOMER_ID");
    log_to_db_with_response_time(ecommerce,
                                 run_num,
                                (char *)"CUSTOMER",
                                 date,
                                 nanos_day,
                                 timeadvance,
                                 pid,
                                 customer_id,
                                 (char *)"GET_CUSTOMER_ID",
                                 duration_ms,
                                 test_type);
    return customer_id;
}