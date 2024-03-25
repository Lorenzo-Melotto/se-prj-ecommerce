#include "Main.h"

int main() {
	 if (DEBUG) printf("Hello, Main!\n");

    Con2DB ecommerce("localhost", "5432", "utente1", "47002", "ecommerce");
    redisContext *ctx = redisConnect("localhost", 6379);

    PGresult *res;
    char sqlcmd[1000] = {0};
    // select the latest run number
    sprintf(sqlcmd, "SELECT MAX(run_number) AS last_run_num FROM logtable;");
    res = ecommerce.ExecSQLtuples(sqlcmd);
    int last_run_num = 0;
    int rows = PQntuples(res);
    if(rows > 0){
        last_run_num = atoi(PQgetvalue(res, 0, PQfnumber(res, "last_run_num")));
    }
    int run_num = last_run_num + 1;
    PQclear(res);
    if (DEBUG) printf("Run number %d\n", run_num);

    // create run directory for output redirection
    char dir_name[255] = {0};
    sprintf(dir_name, "./run%d", run_num);
    mkdir(dir_name, 0777);

    int test_number = 0;
    test(ctx, run_num, ++test_number, 40000, 5, 1, 5, 1, 5, 1, (char *)"furn_5c_1s_cust_5c_1s_tran_5c_1s");
    test(ctx, run_num, ++test_number, 40000, 10, 3, 10, 3, 10, 3, (char *)"furn_10c_3s_cust_10c_3s_tran_10c_3s");
    test(ctx, run_num, ++test_number, 40000, 13, 4, 13, 4, 13, 4, (char *)"furn_13c_4s_cust_13c_4s_tran_13c_4s");
    test(ctx, run_num, ++test_number, 40000, 15, 5, 15, 5, 15, 5, (char *)"furn_15c_5s_cust_15c_5s_tran_15c_5s");
    test(ctx, run_num, ++test_number, 40000, 20, 1, 20, 1, 20, 1, (char *)"furn_20c_1s_cust_20c_1s_tran_20c_1s");
    test(ctx, run_num, ++test_number, 20000, 10, 2, 5, 1, 20, 3, (char *)"furn_10c_2s_cust_5c_1s_tran_20c_3s");

    runMonitors();
    
    ecommerce.close_connection();
    redisFree(ctx);
    return 0;
}
