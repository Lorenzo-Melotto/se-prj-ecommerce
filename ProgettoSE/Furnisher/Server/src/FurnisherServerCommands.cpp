#include "FurnisherServerCommands.h"

void insertItem(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage) {
    // generic message on the stream: INSERT_ITEM item_id QUANTITY quantity FURNISHER furnisher_id
    PGresult *res;
    char sqlcmd[1000] = {0};
    char item_id[255] = {0};      // id of the item
    char quantity[255] = {0};     // quantity available
    char furnisher_id[255] = {0}; // id of the furnisher that sells the item
    char client_id[255] = {0};
    getStreamMessagesValues(rep, numStream, numMessage, 1, item_id);
    getStreamMessagesValues(rep, numStream, numMessage, 3, quantity);
    getStreamMessagesValues(rep, numStream, numMessage, 5, furnisher_id);
    getStreamMessagesValues(rep, numStream, numMessage, 7, client_id);

    if(DEBUG) printf("Furnisher with ID: %s wants to insert item with id %s with quantity %s\n", furnisher_id, item_id, quantity);

    sprintf(sqlcmd, "BEGIN");
    ecommerce.ExecSQLcmd(sqlcmd);

    // create the furnisher
    sprintf(sqlcmd, 
            "INSERT INTO furnishers (id) "
            "VALUES (%s) ON CONFLICT DO NOTHING;", 
            furnisher_id);
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // add the item
    sprintf(sqlcmd, 
            "INSERT INTO items (id, qta, furnisher_id) "
            "VALUES (%s, %s, %s) ON CONFLICT DO NOTHING;", 
            item_id, 
            quantity, 
            furnisher_id);
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    sprintf(sqlcmd, "COMMIT");
    ecommerce.ExecSQLcmd(sqlcmd);

    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %s %s %s %s %s", 
                                         FURNISHER_STREAM_SERVER, 
                                         "OK_INSERT_ITEM", 
                                         "",
                                         "FURNISHER_ID",
                                         furnisher_id,
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void updateItem(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage) {
    PGresult *res;
    char sqlcmd[1000] = {0};
    char item_id[255] = {0};      // id of the item
    char quantity[255] = {0};     // new quantity 
    char furnisher_id[255] = {0}; // id of the furnisher that sells the item
    char client_id[255]= {0};
    getStreamMessagesValues(rep, numStream, numMessage, 1, item_id);
    getStreamMessagesValues(rep, numStream, numMessage, 3, quantity);
    getStreamMessagesValues(rep, numStream, numMessage, 5, furnisher_id);
    getStreamMessagesValues(rep, numStream, numMessage, 7, client_id);

    if (DEBUG) printf("Furnisher with ID: %s wants to update the item with id %s with quantity %s\n", furnisher_id, item_id, quantity);

    //update item quantity
    sprintf(sqlcmd, 
            "UPDATE ITEMS SET qta = %s "
            "WHERE id = %s AND "
            "furnisher_id = %s;", 
            quantity, 
            item_id, 
            furnisher_id);
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %s %s %s %s %s", 
                                         FURNISHER_STREAM_SERVER, 
                                         "OK_UPDATE_ITEM", 
                                         "",
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void listOfItems(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage) {
    char sqlcmd[1000] = {0};
    char furnisher_id[255] = {0};
    char client_id[255] = {0};
    PGresult *res;
    getStreamMessagesValues(rep, numStream, numMessage, 1, furnisher_id); // read furnisher id from the stream
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id); 
    sprintf(sqlcmd, 
            "SELECT id, qta "
            "FROM items "
            "WHERE furnisher_id = %s "
            "ORDER BY RANDOM() "
            "LIMIT 1;", 
            furnisher_id);
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res); // number of rows in the response
    int item_id = -1;
    int item_qta = -1;
    if(rows > 0){
        item_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
        item_qta = atoi(PQgetvalue(res, 0, PQfnumber(res, "qta")));
    }
    PQclear(res);
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %d %s %s", 
                                         FURNISHER_STREAM_SERVER, 
                                         "ITEM_ID", 
                                         item_id, 
                                         "ITEM_QTA", 
                                         item_qta,
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void getFurnisherId(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage) {
    char sqlcmd[1000] = {0};
    char client_id[255] = {0};
    PGresult *res;
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id); 
    sprintf(sqlcmd, 
            "SELECT id "
            "FROM furnishers "
            "ORDER BY RANDOM() "
            "LIMIT 1;");
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res); // number of rows in the response
    int furnisher_id = -1;
    if(rows > 0) {
        furnisher_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    }
    PQclear(res);
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         FURNISHER_STREAM_SERVER, 
                                         "GOT_FURNISHER_ID", 
                                         furnisher_id,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}