#include "CustomerServerCommands.h"

void buyItem(redisContext *ctx,redisReply *rep, Con2DB db, int numStream, int numMessage){
    if (DEBUG) printf("ACTION: BUY_ITEM\n");
    PGresult *res;
    char sqlcmd[1000] = {0};
    char item_id[255] = {0};
    char quantity[255] = {0};
    char customer_id[255] = {0};
    char client_id[255] = {0};
    getStreamMessagesValues(rep, numStream, numMessage, 1, item_id);
    getStreamMessagesValues(rep, numStream, numMessage, 3, quantity);
    getStreamMessagesValues(rep, numStream, numMessage, 5, customer_id);
    getStreamMessagesValues(rep, numStream, numMessage, 7, client_id);
    if(DEBUG) printf("Customer %s trying to buy item with id %s with quantity %s\n", customer_id, item_id, quantity);

    // start a transaction
    sprintf(sqlcmd, "BEGIN");
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // insert the customer ID 
    sprintf(sqlcmd, 
            "INSERT INTO CUSTOMERS (id) "
            "VALUES (%s) ON CONFLICT DO NOTHING;", 
            customer_id);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // insert the order  
    sprintf(sqlcmd, 
            "INSERT INTO ORDERS (customer_id) "
            "VALUES (%s) RETURNING id;", 
            customer_id);
    res = db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    int order_id = -1;
    if(rows > 0){
        order_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
        if(DEBUG) printf("Inserted new order with id %d\n", order_id);
    } else {
        if(DEBUG) printf("No order id received\n");
        // rollback changes
        sprintf(sqlcmd, "ROLLBACK");
        res = db.ExecSQLcmd(sqlcmd);
        PQclear(res);
        return;
    }
    PQclear(res);

    // insert the item_order 
    sprintf(sqlcmd, 
            "INSERT INTO ITEM_ORDER (item_id, order_id, qta) "
            "VALUES (%s, %d, %s);", 
            item_id, 
            order_id, 
            quantity);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // update the item's quantity
    sprintf(sqlcmd, 
            "UPDATE items "
            "SET qta = qta - %s "
            "WHERE id = %s;", 
            quantity, 
            item_id);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res); 

    // commit if everything went well
    sprintf(sqlcmd, "COMMIT");
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);

    //return order_id to client
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         CUSTOMER_STREAM_SERVER, 
                                         "ORDER_ID", 
                                         order_id,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void listOfItems(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage){
    if (DEBUG) printf("ACTION:LISTOFITEMS\n");

    char customer_id[255] = {0};
    char client_id[255]={0};
    getStreamMessagesValues(rep, numStream, numMessage, 3, customer_id);
    getStreamMessagesValues(rep, numStream, numMessage, 5, client_id);

    char sqlcmd[1000] = {0};
    PGresult *res;

    // insert the customer ID 
    sprintf(sqlcmd, 
            "INSERT INTO CUSTOMERS (id) "
            "VALUES (%s) ON CONFLICT DO NOTHING;", 
            customer_id);
    res = db.ExecSQLcmd(sqlcmd);
    PQclear(res);

    //select random item_id
    sprintf(sqlcmd, 
            "SELECT id, qta "
            "FROM items "
            "WHERE qta >= 1 "
            "ORDER BY RANDOM() LIMIT 1;");
    res = db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res); // number of rows in the response
    int item_id = -1;
    int quantity = -1;
    if(rows > 0){
        item_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
        quantity = atoi(PQgetvalue(res, 0, PQfnumber(res, "qta")));
    }
    PQclear(res);
    if(DEBUG) printf("Got item %d with quantity %d\n", item_id, quantity);
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %d %s %s", 
                                         CUSTOMER_STREAM_SERVER, 
                                         "ITEM_ID", 
                                         item_id,
                                         "QUANTITY",
                                         quantity,
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void getCustomerId(redisContext *ctx, redisReply *rep, Con2DB db, int numStream, int numMessage) {
    char sqlcmd[1000] = {0};
    char client_id[255] = {0};
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id);

    PGresult *res;
    sprintf(sqlcmd, 
            "SELECT id "
            "FROM customers "
            "ORDER BY RANDOM() LIMIT 1;");
    res = db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res); // number of rows in the response
    int customer_id = -1;
    if(rows > 0) {
        customer_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    }
    PQclear(res);
    if(DEBUG) printf("Got customer with id %d\n", customer_id);
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         CUSTOMER_STREAM_SERVER, 
                                         "CUSTOMER_ID", 
                                         customer_id,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}