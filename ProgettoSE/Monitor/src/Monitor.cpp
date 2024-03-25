#include "Monitor.h"

int main() {
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);

    Con2DB log_db("localhost", "5432", "utente1", "47002", "ecommerce");

    func_monitor_1(log_db);
    printf("####################\n");
    func_monitor_2(log_db);
    printf("####################\n");
    func_monitor_3(log_db);
    printf("####################\n");
    non_func_monitor_1(log_db);
    printf("####################\n");
    non_func_monitor_2(log_db);
    printf("####################\n");
    printf("Done.\n");

    log_db.close_connection();
    return 0;
}