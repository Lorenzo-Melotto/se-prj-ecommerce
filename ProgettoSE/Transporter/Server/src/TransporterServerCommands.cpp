#include "TransporterServerCommands.h"

void takeOrder(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage){
    char sqlcmd[1000] = {0};
    PGresult *res;    
    char transporter_id[255] = {0};
    char client_id[255] = {0};
    getStreamMessagesValues(rep, numStream, numMessage, 1, transporter_id);
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id);

    sprintf(sqlcmd, "BEGIN");
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    sprintf(sqlcmd, 
            "INSERT INTO transporters (id) "
            "VALUES (%s) ON CONFLICT DO NOTHING;", 
            transporter_id);
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    sprintf(sqlcmd, 
            "UPDATE orders "
            "SET started_delivery = NOW(), transporter_id = %s "
            "WHERE id = (SELECT id FROM orders WHERE started_delivery IS NULL ORDER BY RANDOM() LIMIT 1) RETURNING id;", 
            transporter_id);
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    int order_id = -1;
    if(rows > 0){
        order_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    }
    PQclear(res);

    sprintf(sqlcmd, "COMMIT");
    res = ecommerce.ExecSQLcmd(sqlcmd);
    PQclear(res);

    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         TRANSPORTER_STREAM_SERVER, 
                                         "ORDER_ID_TAKE", 
                                         order_id,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void deliverOrder(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage) {
    char sqlcmd[1000] = {0};
    PGresult *res;
    char transporter_id[255] = {0};
    char client_id[255]= {0};
    getStreamMessagesValues(rep, numStream, numMessage, 1, transporter_id);
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id);

    sprintf(sqlcmd, 
            "UPDATE orders "
            "SET delivery_time = NOW() "
            "WHERE id = (SELECT id FROM orders WHERE transporter_id = %s AND "
            "started_delivery IS NOT NULL AND delivery_time IS NULL ORDER BY started_delivery LIMIT 1) RETURNING id;", 
            transporter_id);
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    int order_id = -1;
    if(rows > 0){
        order_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    }
    PQclear(res);
    if(DEBUG) printf("Delivered order with id %d\n", order_id);
    //Write on stream order_id
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         TRANSPORTER_STREAM_SERVER, 
                                         "ORDER_ID_DELIVERY", 
                                         order_id,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void hasOrders(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage){
    char sqlcmd[1000] = {0};
    PGresult *res;
    char transporter_id[255] = {0};
    char client_id[255] = {0};
    getStreamMessagesValues(rep, numStream, numMessage, 1, transporter_id);
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id);
    
    sprintf(sqlcmd, 
            "SELECT COUNT(*) AS count "
            "FROM orders "
            "WHERE transporter_id = %s AND "
            "delivery_time IS NULL;", 
            transporter_id);
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    int number_of_orders = -1;
    if(rows > 0){
        number_of_orders = atoi(PQgetvalue(res, 0, PQfnumber(res, "count")));
    }
    PQclear(res);

    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         TRANSPORTER_STREAM_SERVER, 
                                         "ORDERS_COUNT", 
                                         number_of_orders,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

void getTransporterId(redisContext *ctx, redisReply *rep, Con2DB ecommerce, int numStream, int numMessage){

    char client_id[255] = {0};
    getStreamMessagesValues(rep, numStream, numMessage, 3, client_id);

    char sqlcmd[1000] = {0};
    double trans_prob = rand_prob();
    if (trans_prob > 0.5f){
        sprintf(sqlcmd, "SELECT t.id "
                        "FROM transporters t, orders o "
                        "WHERE t.id = o.transporter_id AND "
                        "o.delivery_time IS NULL AND "
                        "o.started_delivery IS NOT NULL AND "
                        "o.started_delivery::timestamp <= ALL (SELECT o1.started_delivery::timestamp "
                                                               "FROM orders o1 "
                                                               "WHERE o1.delivery_time IS NULL AND "
                                                                     "o1.started_delivery IS NOT NULL);");
    } else {
        sprintf(sqlcmd, 
                "SELECT transporters.id "
                "FROM transporters, orders "
                "WHERE transporters.id = orders.transporter_id AND "
                "orders.delivery_time IS NULL "
                "ORDER BY RANDOM() LIMIT 1;");
    }

    PGresult *res;
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    if(rows == 0){
        PQclear(res);
        sprintf(sqlcmd, 
                "SELECT transporters.id "
                "FROM transporters, orders "
                "WHERE transporters.id = orders.transporter_id AND "
                "orders.delivery_time IS NULL "
                "ORDER BY RANDOM() LIMIT 1;");
        res = ecommerce.ExecSQLtuples(sqlcmd);
    }
    int transporter_id = -1;
    if(rows > 0) {
        transporter_id = atoi(PQgetvalue(res, 0, PQfnumber(res, "id")));
    }
    PQclear(res);
    redisReply *rep_write = RedisCommand(ctx, 
                                         "XADD %s * %s %d %s %s %s %s", 
                                         TRANSPORTER_STREAM_SERVER, 
                                         "TRANSPORTER_ID", 
                                         transporter_id,
                                         "FILLER",
                                         "",
                                         "CLIENT_ID",
                                         client_id);
    assertReplyType(ctx, rep_write, REDIS_REPLY_STRING); 
    freeReplyObject(rep_write);
}

