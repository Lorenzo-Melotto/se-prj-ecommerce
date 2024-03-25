#include "Tests.h"

void test(redisContext *ctx, int run_num, int test_num, int time_to_wait, 
          int furn_client_num, int furn_server_num, int cust_client_num, 
          int cust_server_num, int tran_client_num, int tran_server_num, 
          char *test_name){
    if(40000 % time_to_wait != 0){
        fprintf(stderr, "Time to wait must be a divisor or a multiple of 40000\n");
        exit(1);
    }
    printf("Running test %s. Run number: %d\n", test_name, run_num);

    // delete FURNISHER streams 
    redisReply *rep;
    rep = RedisCommand(ctx, "DEL %s", FURNISHER_STREAM_CLIENT);
    assertReply(ctx, rep);
    rep = RedisCommand(ctx, "DEL %s", FURNISHER_STREAM_SERVER);
    assertReply(ctx, rep);
    freeReplyObject(rep);

    // delete CUSTOMER streams 
    rep = RedisCommand(ctx, "DEL %s", CUSTOMER_STREAM_CLIENT);
    assertReply(ctx, rep);
    rep = RedisCommand(ctx, "DEL %s", CUSTOMER_STREAM_SERVER);
    assertReply(ctx, rep);
    freeReplyObject(rep);

    // delete TRANSPORTER streams 
    rep = RedisCommand(ctx, "DEL %s", TRANSPORTER_STREAM_CLIENT);
    assertReply(ctx, rep);
    rep = RedisCommand(ctx, "DEL %s", TRANSPORTER_STREAM_SERVER);
    assertReply(ctx, rep);
    freeReplyObject(rep);

    // initialize all the streams
    initStream(ctx, FURNISHER_STREAM_CLIENT);
    initStream(ctx, FURNISHER_STREAM_SERVER);

    initStream(ctx, CUSTOMER_STREAM_CLIENT);
    initStream(ctx, CUSTOMER_STREAM_SERVER);

    initStream(ctx, TRANSPORTER_STREAM_CLIENT);
    initStream(ctx, TRANSPORTER_STREAM_SERVER);

    init_rng_with_time();

    char *cmd_args[255] = {0};
    vector<pid_t> server_pids;
    vector<FILE *> file_descriptors;    
    pid_t pid_server;
    // FURNISHER
    for(int i = 0; i < furn_server_num; i++){
        char filename[255] = {0};
        sprintf(filename, "./run%d/test%d_furnisher_server%d.txt", run_num, test_num, i+1);
        FILE *fd = fopen(filename, "w+");
        if(fd == NULL){
            printf("Errore nell'apertura del file di reindirizzamento: %s\n", strerror(errno));
            exit(1);
        }
        file_descriptors.push_back(fd);

        pid_server = fork();
        server_pids.push_back(pid_server);
        if(pid_server == 0){
            printf("Spinning up furnisher server %d with pid %d.\n", i + 1, getpid());
            close(STDOUT_FILENO);
            dup2(fileno(fd), STDOUT_FILENO);;
            // spawn the server
            if(execvp("../../Furnisher/Server/bin/FurnisherServer", (char **)cmd_args) < 0){
                fclose(fd);
                fprintf(stderr, "Errore nell'esecuzione del server: %s\n", strerror(errno));
                exit(1);
            }
            exit(0);
        }
        millisec_sleep(10);
    }
    
    // CUSTOMER
    for(int i = 0; i < cust_server_num; i++){
        char filename[255] = {0};
        sprintf(filename, "./run%d/test%d_customer_server%d.txt", run_num, test_num, i+1);
        FILE *fd = fopen(filename, "w+");
        if(fd == NULL){
            printf("Errore nell'apertura del file di reindirizzamento: %s\n", strerror(errno));
            exit(1);
        }
        file_descriptors.push_back(fd);

        pid_server = fork();
        server_pids.push_back(pid_server);
        if(pid_server == 0){
            printf("Spinning up customer server %d with pid %d.\n", i + 1, getpid());
            close(STDOUT_FILENO);
            dup2(fileno(fd), STDOUT_FILENO);;
            // spawn the server
            if(execvp("../../Customer/Server/bin/CustomerServer", (char **)cmd_args) < 0){
                fclose(fd);
                fprintf(stderr, "Errore nell'esecuzione del server: %s\n", strerror(errno));
                exit(1);
            }
            exit(0);
        }
        millisec_sleep(10);
    }

    // TRANSPORTER
    for(int i = 0; i < tran_server_num; i++){
        char filename[255] = {0};
        sprintf(filename, "./run%d/test%d_transporter_server%d.txt", run_num, test_num, i+1);
        FILE *fd = fopen(filename, "w+");
        if(fd == NULL){
            printf("Errore nell'apertura del file di reindirizzamento: %s\n", strerror(errno));
            exit(1);
        }
        file_descriptors.push_back(fd);

        pid_server = fork();
        server_pids.push_back(pid_server);
        if(pid_server == 0){
            printf("Spinning up transporter server %d with pid %d.\n", i + 1, getpid());
            close(STDOUT_FILENO);
            dup2(fileno(fd), STDOUT_FILENO);;
            // child process
            printf("Child server with pid %d\n", getpid());
            // spawn the server
            if(execvp("../../Transporter/Server/bin/TransporterServer", (char **)cmd_args) < 0){
                fclose(fd);
                fprintf(stderr, "Errore nell'esecuzione del server: %s\n", strerror(errno));
                exit(1);
            }
            exit(0);
        }
        millisec_sleep(10);
    }

    // FURNISHER
    char ttw[255] = {0};
    sprintf(ttw, "%d", time_to_wait);
    for(int i = 0; i < furn_client_num; i++){
        char filename[255] = {0};
        sprintf(filename, "./run%d/test%d_furnisher_client%d.txt", run_num, test_num, i+1);
        FILE *fd = fopen(filename, "w+");
        if(fd == NULL){
            printf("Errore nell'apertura del file di reindirizzamento: %s\n", strerror(errno));
            exit(1);
        }
        file_descriptors.push_back(fd);

        char run_num_str[255] = {0};
        sprintf(run_num_str, "%d", run_num);
        char *args[255] = {(char *)"-t", 
                           (char *)ttw, 
                           (char *)"-r", 
                           run_num_str, 
                           (char *)"-n", 
                           test_name, 
                           NULL};
        pid_t client_pid;
        if((client_pid = fork()) == 0){
            printf("Spinning up furnisher client %d with pid %d.\n", i + 1, getpid());
            close(STDOUT_FILENO);
            dup2(fileno(fd), STDOUT_FILENO);;
            if(execvp("../../Furnisher/Client/bin/FurnisherClient", args) < 0){
                fclose(fd);
                fprintf(stderr, "Errore nell'esecuzione del client: %s\n", strerror(errno));
                exit(1);
            }
            exit(0);
        }
        millisec_sleep(rand_range(100, 2000));
    }

    // CUSTOMER
    for(int i = 0; i < cust_client_num; i++){
        char filename[255] = {0};
        sprintf(filename, "./run%d/test%d_customer_client%d.txt", run_num, test_num, i+1);
        FILE *fd = fopen(filename, "w+");
        if(fd == NULL){
            printf("Errore nell'apertura del file di reindirizzamento: %s\n", strerror(errno));
            exit(1);
        }
        file_descriptors.push_back(fd);

        char run_num_str[255] = {0};
        sprintf(run_num_str, "%d", run_num);
        char *args[255] = {(char *)"-t", 
                           (char *)ttw,
                           (char *)"-r", 
                           run_num_str, 
                           (char *)"-n", 
                           test_name, 
                           NULL};
        pid_t client_pid;
        if((client_pid = fork()) == 0){
            printf("Spinning up customer client %d with pid %d.\n", i + 1, getpid());
            close(STDOUT_FILENO);
            dup2(fileno(fd), STDOUT_FILENO);;
            if(execvp("../../Customer/Client/bin/CustomerClient", args) < 0){
                fclose(fd);
                fprintf(stderr, "Errore nell'esecuzione del client: %s\n", strerror(errno));
                exit(1);
            }
            exit(0);
        }
        millisec_sleep(rand_range(100, 2000));
    }

    // TRANSPORTER 
    for(int i = 0; i < tran_client_num; i++){
        char filename[255] = {0};
        sprintf(filename, "./run%d/test%d_transporter_client%d.txt", run_num, test_num, i+1);
        FILE *fd = fopen(filename, "w+");
        if(fd == NULL){
            printf("Errore nell'apertura del file di reindirizzamento: %s\n", strerror(errno));
            exit(1);
        }
        file_descriptors.push_back(fd);

        char run_num_str[255] = {0};
        sprintf(run_num_str, "%d", run_num);
        char *args[255] = {(char *)"-t", 
                           (char *)ttw, 
                           (char *)"-r", 
                           run_num_str, 
                           (char *)"-n", 
                           test_name, 
                           NULL};
        pid_t client_pid;
        if((client_pid = fork()) == 0){
            printf("Spinning up transporter client %d with pid %d.\n", i + 1, getpid());
            close(STDOUT_FILENO);
            dup2(fileno(fd), STDOUT_FILENO);;
            if(execvp("../../Transporter/Client/bin/TransporterClient", args) < 0){
                fclose(fd);
                fprintf(stderr, "Errore nell'esecuzione del client: %s\n", strerror(errno));
                exit(1);
            }
            exit(0);
        }
        millisec_sleep(rand_range(100, 2000));
    }

    // close all the file descriptors opened by the clients
    for(FILE *f : file_descriptors){
        fclose(f);
    }

    // wait for clients termination
    for(int i = 0; i < furn_client_num + cust_client_num + tran_client_num; i++){
        // parent process waits for children termination
        pid_t term = wait(NULL);
        printf("Transporter client with pid %d terminated execution.\n", term);
    }

    for(pid_t p : server_pids){
        printf("Killing server with pid: %d\n", p);
        kill(p, SIGTERM);
        millisec_sleep(100);
        int status = -1;
        if(waitpid(p, &status, WNOHANG) == 0){
            kill(p, SIGKILL);
        }
        printf("Server with pid %d exited with exit code %d.\n", p, status);
    }
    printf("Test %s finished.\n", test_name);
    millisec_sleep(1000);
}

void runMonitors(){
    pid_t monitor_pid;
    char *args[255] = {0};
    if((monitor_pid = fork()) == 0){
        if(execv("../../Monitor/bin/Monitor", args) < 0){
            fprintf(stderr, "Spawning monitor process failed: %s\n", strerror(errno));
            exit(1);
        }
        exit(0);
    }
    int status = 0;
    if(waitpid(monitor_pid, &status, 0) < 0){
        fprintf(stderr, "Error in wait: %s\n", strerror(errno));
        exit(1);
    }
    printf("Monitor process with pid %d terminated execution with exit code: %d\n", monitor_pid, status);
}
