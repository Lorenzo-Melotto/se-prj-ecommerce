#include "CustomerClient.h"

int main(int argc, char *argv[]) {
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);

    int time_to_sleep = 40000;
    int run_num = 0; 
    char test_type[255] = {0};
    read_args(argc, argv, time_to_sleep, run_num, test_type);

    if (DEBUG) printf("Hello customer client!\n");

    redisContext *ctx;
    Con2DB ecommerce("localhost", "5432", "utente1", "47002", "ecommerce");
    int block = 100000000;
    int pid = getpid();

	ctx = redisConnect("localhost", 6379);

    unsigned int seed = init_rng_with_time();
    char user_stream_id[255] = {0};
    sprintf(user_stream_id, "%d", rand()); // random user for stream read            

    init_time();
    nanos = get_nanoseconds();
    int ticks = 0;

	while(ticks <= HORIZON){
	    double create_customer_prob = rand_prob(); 
	    int customer_id = -1;
		if(create_customer_prob > 0.5f) {
			// create a random customer, simulates a new account creation 
			customer_id = rand();
			if (DEBUG) printf("New Customer with id: %d\n", customer_id);
		} else {
			// get the customer id from the server
			customer_id = getCustomerId(ctx, user_stream_id, block, ecommerce, pid, run_num, test_type);
		}
		// the customer client tries only to buy an item
        int num_of_actions = rand_range(2, 10);
        if(DEBUG) printf("Customer %d decides to take %d actions\n", customer_id, num_of_actions);
        for(int i = 0; i < num_of_actions; i++){
            double buy_prob = rand_prob();
            if(buy_prob > 0.7f){
                // buy something
                if(customer_id < 0) customer_id = rand();
                buyItem(ctx, user_stream_id, block, customer_id, ecommerce, pid, run_num, test_type);
            } else {
                if (DEBUG) printf("No action\n");
            }
            millisec_sleep(30); // waits 1 simulated minute between actions
        }
        ticks++;
        millisec_sleep(time_to_sleep - 30 * num_of_actions);
        update_time();

        if (DEBUG) printf("##############################\n");
 	}
    redisFree(ctx);
    ecommerce.close_connection();
    if(DEBUG) printf("Goodbye customer client!\n");
    return 0;
}
