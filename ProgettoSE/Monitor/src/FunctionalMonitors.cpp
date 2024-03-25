#include "FunctionalMonitors.h"

void func_monitor_1(Con2DB log_db){
    printf("Functional monitor 1 running...\n");
    char sqlcmd[1000] = {0};
    PGresult *select_res;
    sprintf(sqlcmd,
            "SELECT * "
            "FROM item_quantity_history, logtable "
            "WHERE item_quantity_history.order_id = logtable.order_id AND "
                  "func_monitor_1 IS NULL AND "
                  "actor_action = '%s';", 
            "BUY_ITEM");
    select_res = log_db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(select_res);
    if(rows > 0){
        for(int i = 0; i < rows; i++){
            char *timevalue =       PQgetvalue(select_res, i, PQfnumber(select_res, "timevalue"));
            char *nanoseconds =     PQgetvalue(select_res, i, PQfnumber(select_res, "nanoseconds"));
            char *pid =             PQgetvalue(select_res, i, PQfnumber(select_res, "pid"));
            char *customer_id =     PQgetvalue(select_res, i, PQfnumber(select_res, "customer_id"));
            int new_quantity = atoi(PQgetvalue(select_res, i, PQfnumber(select_res, "new_quantity")));
            if(new_quantity < 0){
                sprintf(sqlcmd, 
                        "UPDATE logtable "
                        "SET func_monitor_1='t' "
                        "WHERE timevalue = '%s' AND "
                              "nanoseconds = %s AND "
                              "pid = %s AND "
                              "customer_id = %s;", 
                        timevalue, 
                        nanoseconds, 
                        pid, 
                        customer_id);
            } else {
                sprintf(sqlcmd, 
                        "UPDATE logtable "
                        "SET func_monitor_1='f' "
                        "WHERE timevalue = '%s' AND "
                              "nanoseconds = %s AND "
                              "pid = %s AND "
                              "customer_id = %s;", 
                        timevalue, 
                        nanoseconds, 
                        pid, 
                        customer_id);
            }
            log_db.ExecSQLcmd(sqlcmd);
        }
    } else {
        printf("Nessun risultato\n");
    }
    PQclear(select_res);
}

void func_monitor_2(Con2DB log_db){
    printf("Functional monitor 2 running...\n");
    char sqlcmd[1000] = {0};
    PGresult *select_res;
    // select EXTRACT(EPOCH FROM (delivery_time - started_delivery)) as delivery_diff from orders;
    sprintf(sqlcmd,
            "SELECT *, EXTRACT(EPOCH FROM (delivery_time - started_delivery)) AS delivery_diff "
            "FROM logtable, orders "
            "WHERE actor_action = '%s' AND "
                "func_monitor_2 IS NULL AND "
                "logtable.order_id = orders.id", 
            "DELIVER_ORDER");
    select_res = log_db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(select_res);
    if(rows > 0){
        for(int i = 0; i < rows; i++){
            char *timevalue =        PQgetvalue(select_res, i, PQfnumber(select_res, "timevalue"));
            char *nanoseconds =      PQgetvalue(select_res, i, PQfnumber(select_res, "nanoseconds"));
            char *pid =              PQgetvalue(select_res, i, PQfnumber(select_res, "pid"));
            char *transporter_id =   PQgetvalue(select_res, i, PQfnumber(select_res, "transporter_id"));
            int delivery_diff = atoi(PQgetvalue(select_res, i, PQfnumber(select_res, "delivery_diff"))); 
            if(delivery_diff < 0 || delivery_diff > 280){
                sprintf(sqlcmd, 
                        "UPDATE logtable "
                        "SET func_monitor_2 = 't' "
                        "WHERE timevalue = '%s' AND "
                              "nanoseconds = %s AND "
                              "pid = %s AND "
                              "transporter_id = %s;", 
                        timevalue, 
                        nanoseconds, 
                        pid, 
                        transporter_id);
            } else {
                sprintf(sqlcmd, 
                        "UPDATE logtable "
                        "SET func_monitor_2 = 'f' "
                        "WHERE timevalue = '%s' AND "
                              "nanoseconds = %s AND "
                              "pid = %s AND "
                              "transporter_id = %s;", 
                        timevalue, 
                        nanoseconds, 
                        pid, 
                        transporter_id);
            }
            log_db.ExecSQLcmd(sqlcmd);
        }
    } else {
        printf("Nessun risultato\n");
    }
    PQclear(select_res);
}

void func_monitor_3(Con2DB log_db){
    printf("Functional monitor 3 running...\n");
    char sqlcmd[1000] = {0};
    PGresult *select_res;
    PGresult *select_error;
    //trovo le buy andate a buon fine e setto monitor a false
    sprintf(sqlcmd,
            "SELECT timevalue, nanoseconds, pid "
            "FROM logtable, orders "
            "WHERE logtable.order_id = orders.id AND "
            "func_monitor_3 IS NULL AND "
                "actor_action = '%s';",
            "BUY_ITEM");
    select_res = log_db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(select_res);
    if(rows > 0){
        for(int i = 0; i < rows; i++){
            char *timevalue =        PQgetvalue(select_res, i, PQfnumber(select_res, "timevalue"));
            char *nanoseconds =      PQgetvalue(select_res, i, PQfnumber(select_res, "nanoseconds"));
            char *pid =              PQgetvalue(select_res, i, PQfnumber(select_res, "pid"));
            sprintf(sqlcmd, 
                "UPDATE logtable "
                "SET func_monitor_3 = 'f' "
                "WHERE timevalue = '%s' AND "
                      "nanoseconds = %s AND "
                      "pid = %s;", 
                timevalue, 
                nanoseconds, 
                pid);
            log_db.ExecSQLcmd(sqlcmd);  
        }      
    } else{
        printf("Nessun risultato\n");
    }
    //trovo le buy non andate a buon fine e setto il monitor a true
    sprintf(sqlcmd,
            "(SELECT timevalue, nanoseconds, pid "
            "FROM logtable l "
            "WHERE l.func_monitor_3 IS NULL AND "
            "l.actor_action = 'BUY_ITEM') "
            "EXCEPT "
            "(SELECT timevalue, nanoseconds, pid "
            "FROM logtable l, orders o "
            "WHERE l.order_id = o.id AND "
            "l.func_monitor_3 IS NULL AND "
            "l.actor_action = 'BUY_ITEM');");
    select_error = log_db.ExecSQLtuples(sqlcmd);
    rows = PQntuples(select_error);
    if(rows > 0){
        for(int i = 0; i < rows; i++){
            char *timevalue =        PQgetvalue(select_error, i, PQfnumber(select_error, "timevalue"));
            char *nanoseconds =      PQgetvalue(select_error, i, PQfnumber(select_error, "nanoseconds"));
            char *pid =              PQgetvalue(select_error, i, PQfnumber(select_error, "pid"));
            sprintf(sqlcmd, 
                "UPDATE logtable "
                "SET func_monitor_3 = 't' "
                "WHERE timevalue = '%s' AND "
                        "nanoseconds = %s AND "
                        "pid = %s AND", 
                timevalue, 
                nanoseconds, 
                pid);
            log_db.ExecSQLcmd(sqlcmd);  
        }      
    } else{
        printf("Nessun risultato\n");
    }
    PQclear(select_res);
    PQclear(select_error);
}
