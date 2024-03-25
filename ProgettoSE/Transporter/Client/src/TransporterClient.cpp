#include "TransporterClient.h"

int main(int argc, char *argv[]) {
	setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);

    int time_to_sleep = 40000;
    int run_num = 0; 
    char test_type[255] = {0};
    read_args(argc, argv, time_to_sleep, run_num, test_type);

	if (DEBUG) printf("Hello, transporter client!\n");

	redisContext *ctx;
    Con2DB ecommerce("localhost", "5432", "utente1", "47002", "ecommerce");
	int block = 100000000;
    int pid = getpid();
    
	ctx = redisConnect("localhost", 6379);

    unsigned int seed = init_rng_with_time();
	char user_stream_id[255] = {0};
    sprintf(user_stream_id, "%d", rand());      

    init_time();
    nanos = get_nanoseconds();    
    int ticks = 0; 

	while(ticks <= HORIZON){
        int transporter_id = -1;
    	double create_trans_prob = rand_prob(); 	
        bool is_new_transporter = false;
		if(create_trans_prob > 0.5f) {
        	// create a random transporter
        	transporter_id = rand();
            is_new_transporter = true;
        	if(DEBUG) printf("New transporter with id: %d\n", transporter_id);
        } else {
            // get the transporter id from the server
            transporter_id = getTransporterId(ctx, user_stream_id, block, ecommerce, pid, run_num, test_type);
        }

        int num_of_actions = rand_range(10, 20);
        if(DEBUG) printf("Transporter %d decides to take %d actions\n", transporter_id, num_of_actions);
        for(int i = 0; i < num_of_actions; i++){
		    double action_prob = rand_prob();
            if(action_prob < 0.25f){ 
                //accept a order delivery
                if(transporter_id < 0) transporter_id = rand();
                is_new_transporter = false;
                takeOrder(ctx, transporter_id, user_stream_id, block, ecommerce, pid, run_num, test_type);
            } else if(action_prob >= 0.25f && action_prob <= 0.9f) {
                //deliver a random order
                if(is_new_transporter || transporter_id == -1) {
                    // if the tranporter is new, it does not have orders to deliver
                    if(DEBUG) printf("[WARNING] New transporters cannot deliver orders as their first action\n");
                    millisec_sleep(30); // waits 1 simulated minute between actions
                    continue;  
                }
                deliverOrder(ctx, user_stream_id, block, transporter_id, ecommerce, pid, run_num, test_type);
            } else {
                if (DEBUG) printf("No action\n");
            }
            millisec_sleep(30); // waits 1 simulated minute between actions
        }
        ticks++;
        millisec_sleep(time_to_sleep - 30 * num_of_actions);
        update_time();

        if (DEBUG) printf("####################\n");      
	}
	redisFree(ctx);
    ecommerce.close_connection();
    if (DEBUG) printf("Goodbye transporter client!\n");
	return 0;
}
