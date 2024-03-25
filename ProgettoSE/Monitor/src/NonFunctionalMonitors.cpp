#include "NonFunctionalMonitors.h"

void non_func_monitor_1(Con2DB log_db){
    printf("Non functional monitor 1 running ...\n");
    char sqlcmd[1000] = {0};
    PGresult *select_res;
    sprintf(sqlcmd,
            "SELECT * "
            "FROM logtable "
            "WHERE non_func_monitor_1 IS NULL AND "
            "actor_action::TEXT LIKE 'GET_%%';");
    select_res = log_db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(select_res);
    if(rows > 0){
        // check
        for(int i = 0; i < rows; i++){
            char *timevalue =             PQgetvalue(select_res, i, PQfnumber(select_res, "timevalue"));
            char *nanoseconds =           PQgetvalue(select_res, i, PQfnumber(select_res, "nanoseconds"));
            char *pid =                   PQgetvalue(select_res, i, PQfnumber(select_res, "pid"));
            float response_time_ms = atof(PQgetvalue(select_res, i, PQfnumber(select_res, "response_time_ms")));
            if(response_time_ms >= 46.0f * 3){
                sprintf(sqlcmd, 
                        "UPDATE logtable "
                        "SET non_func_monitor_1 = 't' "
                        "WHERE timevalue = '%s' AND "
                        "nanoseconds = %s AND "
                        "pid = %s;",
                        timevalue, 
                        nanoseconds, 
                        pid);
            } else {
                sprintf(sqlcmd, 
                        "UPDATE logtable "
                        "SET non_func_monitor_1 = 'f' "
                        "WHERE timevalue = '%s' AND "
                        "nanoseconds = %s AND "
                        "pid = %s;",
                        timevalue, 
                        nanoseconds, 
                        pid);
            }
            log_db.ExecSQLcmd(sqlcmd); // monitor write to DB
        }
    } else {
        printf("Nessun risultato\n");
    }
    PQclear(select_res);
}

void non_func_monitor_2 (Con2DB log_db){
    printf("Non functional monitor 2 running ...\n");
    char sqlcmd[1000] = {0};
    PGresult *rem_runs;
    PGresult *simulation_time_select;
    PGresult *ratio_select;

    sprintf(sqlcmd, 
            "(SELECT DISTINCT run_number "
            "FROM logtable) "
            "EXCEPT "
            "(SELECT run_number "
            "FROM transactions_per_second) "
            "ORDER BY run_number;");
    rem_runs = log_db.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(rem_runs);
    if(rows > 0){
        for (int i=0; i<rows; i++){
            char *run_number = PQgetvalue(rem_runs, i, PQfnumber(rem_runs, "run_number"));
            //find the start_simulation_time dell'actual run
            sprintf(sqlcmd, "SELECT run_number, timevalue, nanoseconds "
                            "FROM logtable "
                            "WHERE run_number = %s " 
                            "ORDER BY timevalue;",
                            run_number);
            simulation_time_select = log_db.ExecSQLtuples(sqlcmd);
            int rows2 = PQntuples(simulation_time_select);
            //estraggo oldest timevalue actual run
            char *start_timestamp = PQgetvalue(simulation_time_select, 0, PQfnumber(simulation_time_select, "timevalue"));
            char *start_nanosecond = PQgetvalue(simulation_time_select, 0, PQfnumber(simulation_time_select, "nanoseconds"));
            //estraggo newest timevalue actual run
            char *end_timestamp = PQgetvalue(simulation_time_select, rows2 - 1, PQfnumber(simulation_time_select, "timevalue"));
            char *end_nanosecond = PQgetvalue(simulation_time_select, rows2 - 1, PQfnumber(simulation_time_select, "nanoseconds"));
            //select transaction number/seconds passed from start to end
            sprintf(sqlcmd, 
                    "SELECT COUNT(*)/EXTRACT(EPOCH FROM (TIMESTAMP '%s' - TIMESTAMP '%s' )) AS ratio " 
                    "FROM logtable "
                    "WHERE run_number = %s;",
                    end_timestamp,
                    start_timestamp,
                    run_number);
            ratio_select = log_db.ExecSQLtuples(sqlcmd);
            double ratio = atof(PQgetvalue(ratio_select, 0, PQfnumber(ratio_select, "ratio")));
            if(ratio < 3.0f){
                // non va bene!
                sprintf(sqlcmd, 
                        "INSERT INTO transactions_per_second " 
                        "(run_number, start_timestamp, start_nanoseconds, end_timestamp, end_nanoseconds, transaction_s, monitor_response) "
                        "VALUES (%s, '%s', '%s', '%s', '%s', %f, 't');", 
                        run_number,
                        start_timestamp,
                        start_nanosecond,
                        end_timestamp,
                        end_nanosecond,
                        ratio);
            } else {
                // va bene!
                sprintf(sqlcmd,
                        "INSERT INTO transactions_per_second " 
                        "(run_number, start_timestamp, start_nanoseconds, end_timestamp, end_nanoseconds, transaction_s, monitor_response) "
                        "VALUES (%s, '%s', '%s', '%s', '%s', %f, 'f');", 
                        run_number,
                        start_timestamp,
                        start_nanosecond,
                        end_timestamp,
                        end_nanosecond,
                        ratio);
            }
            log_db.ExecSQLcmd(sqlcmd);
        } 
        PQclear(ratio_select);
        PQclear(simulation_time_select);
    }
    else{
        printf("No runs to check\n");
    }
    PQclear(rem_runs);
}
