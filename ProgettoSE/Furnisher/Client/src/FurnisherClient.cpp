#include "FurnisherClient.h"

int main(int argc, char *argv[]) {
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);

    int time_to_sleep = 40000;
    int run_num = 0; 
    char test_type[255] = {0};
    read_args(argc, argv, time_to_sleep, run_num, test_type);

    if(DEBUG) printf("Hello, furnisher client!\n");
    
	redisContext *ctx; // contesto per la comunicazione con redis
    Con2DB ecommerce("localhost", "5432", "utente1", "47002", "ecommerce");
    int block = 100000000;
    int pid = getpid();

	ctx = redisConnect("localhost", 6379);
    
    unsigned int seed = init_rng_with_time();
    char user_stream_id[255] = {0};
    sprintf(user_stream_id, "%d", rand()); // random user for stream read            
    if(DEBUG) printf("Seed: %d\n", seed);
    
    init_time();
    nanos = get_nanoseconds();
    int ticks = 0;

    while(ticks <= HORIZON){
        int furnisher_id = -1;
        double create_furn_prob = rand_prob(); 
        bool is_new_furnisher = false;
        if(create_furn_prob > 0.5f) {
            // create a random furnisher
            furnisher_id = rand();
            is_new_furnisher = true;
            if(DEBUG) printf("New furnisher with id: %d\n", furnisher_id);
        } else {
            // get the furnisher id from the server
            furnisher_id = getFurnisherId(ctx, user_stream_id, block, ecommerce, pid, run_num, test_type);
        }
        
        int num_of_actions = rand_range(2, 10);
        if(DEBUG) printf("Furnisher %d decides to take %d actions\n", furnisher_id, num_of_actions);
        for(int i = 0; i < num_of_actions; i++){
            double action_prob = rand_prob();
            if(action_prob < 0.45f){ 
                //insert new product
                if(furnisher_id < 0) furnisher_id = rand();
                is_new_furnisher = false;
                insertItem(ctx, furnisher_id, user_stream_id, block, ecommerce, pid, run_num, test_type);
            } else if(action_prob >= 0.45f && action_prob < 0.9f) {
                if(is_new_furnisher || furnisher_id < 0) {
                    millisec_sleep(30); // waits 1 simulated minute between actions
                    continue;
                }
                updateItem(ctx, user_stream_id, block, furnisher_id, ecommerce, pid, run_num, test_type);
            } else {
                if(DEBUG) printf("No action\n");
            }
            millisec_sleep(30); // waits 1 simulated minute between actions
        }
        ticks++;
        millisec_sleep(time_to_sleep - 30 * num_of_actions);
        update_time();

        if(DEBUG) printf("####################\n");
    }

	redisFree(ctx);
    ecommerce.close_connection();
    if(DEBUG) printf("Goodbye furnisher client!\n");
    return 0;
}
