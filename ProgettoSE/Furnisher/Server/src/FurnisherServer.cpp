#include "FurnisherServer.h"

int main() {
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);

	redisContext *ctx; 
    redisReply *rep; 
    int block = 100000000; 

    Con2DB ecommerce("localhost", "5432", "utente1", "47002", "ecommerce");

	ctx = redisConnect("localhost", 6379);
    
    unsigned int seed = init_rng_with_time();
    char user_stream_id[255] = {0};
    sprintf(user_stream_id, "%d", rand()); // random user for stream read

	while(1){
        rep = readFromStream(ctx, (char *)"messages", user_stream_id, block, FURNISHER_STREAM_CLIENT);
		for (int numStream = 0; numStream < getNumStreams(rep); numStream++) {
            char msg_id[255] = {0};
			for (int numMessage = 0; numMessage < getStreamLen(rep, numStream); numMessage++) { 
                readStreamMessageId(rep, numStream, numMessage, msg_id);
                char key[255] = {0};
                getStreamMessagesValues(rep, numStream, numMessage, 0, key); // the first key contains the command to be executed 
                if(strcmp(key, "INSERT_ITEM") == 0){
                    insertItem(ctx, rep, ecommerce, numStream, numMessage);
                    // break;
                } else if(strcmp(key, "UPDATE_ITEM") == 0) {
                    updateItem(ctx, rep, ecommerce, numStream, numMessage);
                    // break;
                } else if(strcmp(key, "LIST_OF_ITEMS") == 0) {
                    listOfItems(ctx, rep, ecommerce, numStream, numMessage);
                    // break;
                } else if(strcmp(key, "GET_FURNISHER_ID") == 0) {
                    getFurnisherId(ctx, rep, ecommerce, numStream, numMessage);
                    // break;
                } else {
                    if(DEBUG) printf("Invalid command: %s\n", key);
                    // break;
                }
                ackMessage(ctx, FURNISHER_STREAM_CLIENT, msg_id);
		    }
		}
        freeReplyObject(rep);
        if(DEBUG) printf("####################\n");
	}
	redisFree(ctx);
    ecommerce.close_connection();
    return 0;
}