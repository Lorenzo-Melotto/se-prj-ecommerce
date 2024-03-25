#include "log_to_db.h"

void log_to_db_with_response_time(Con2DB db, int run_number, char *actor, char *date, long nanos_day, double timeadvance, int pid, int actor_id, char *actor_action, int action_object_id, float response_time_ms, char *test_type){
    char sqlcmd[1000] = {0};
    if(strcmp(actor, "CUSTOMER") == 0){
        sprintf(sqlcmd, 
                "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, customer_id, actor_action, item_id, response_time_ms, test_type) "
                "VALUES (%d, '%s', %ld, %d, %d, '%s', %d, %f, '%s') ON CONFLICT DO NOTHING;",
                run_number,
                date,
                nanos_day,
                pid,
                actor_id,
                actor_action,
                action_object_id,
                response_time_ms,
                test_type);
    } else if(strcmp(actor, "FURNISHER") == 0){
        sprintf(sqlcmd, 
                "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, furnisher_id, actor_action, item_id, response_time_ms, test_type) "
                "VALUES (%d, '%s', %ld, %d, %d, '%s', %d, %f, '%s') ON CONFLICT DO NOTHING;",
                run_number,
                date,
                nanos_day,
                pid,
                actor_id,
                actor_action,
                action_object_id,
                response_time_ms,
                test_type);
    } else if(strcmp(actor, "TRANSPORTER") == 0){
        sprintf(sqlcmd, 
                "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, transporter_id, actor_action, order_id, response_time_ms, test_type) "
                "VALUES (%d, '%s', %ld, %d, %d, '%s', %d, %f, '%s') ON CONFLICT DO NOTHING;",
                run_number,
                date,
                nanos_day,
                pid,
                actor_id,
                actor_action,
                action_object_id,
                response_time_ms,
                test_type);
    }
    db.ExecSQLcmd(sqlcmd);
}

void log_to_db_with_response_time(Con2DB db, int run_number, char *actor, char *date, long nanos_day, double timeadvance, int pid, int actor_id, char *actor_action, float response_time_ms, char *test_type){
    char sqlcmd[1000] = {0};
    if(strcmp(actor, "CUSTOMER") == 0){
        sprintf(sqlcmd, 
                "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, customer_id, actor_action, response_time_ms, test_type) "
                "VALUES (%d, '%s', %ld, %d, %d, '%s', %f, '%s') ON CONFLICT DO NOTHING;",
                run_number,
                date,
                nanos_day,
                pid,
                actor_id,
                actor_action,
                response_time_ms,
                test_type);
    } else if(strcmp(actor, "FURNISHER") == 0){
        sprintf(sqlcmd, 
                "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, furnisher_id, actor_action, response_time_ms, test_type) "
                "VALUES (%d, '%s', %ld, %d, %d, '%s', %f, '%s') ON CONFLICT DO NOTHING;",
                run_number,
                date,
                nanos_day,
                pid,
                actor_id,
                actor_action,
                response_time_ms,
                test_type);
    } else if(strcmp(actor, "TRANSPORTER") == 0){
        sprintf(sqlcmd, 
                "INSERT INTO logtable (run_number, timevalue, nanoseconds, pid, transporter_id, actor_action, response_time_ms, test_type) "
                "VALUES (%d, '%s', %ld, %d, %d, '%s', %f, '%s') ON CONFLICT DO NOTHING;",
                run_number,
                date,
                nanos_day,
                pid,
                actor_id,
                actor_action,
                response_time_ms,
                test_type);
    }
    db.ExecSQLcmd(sqlcmd);
}