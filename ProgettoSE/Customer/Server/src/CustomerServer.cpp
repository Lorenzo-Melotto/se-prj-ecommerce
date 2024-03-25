#include "CustomerServer.h"

int main() {
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);
	if (DEBUG) printf("Hello, Server!\n");

    redisContext *ctx; 
    redisReply *rep; 
    int block = 100000000; 

    Con2DB db("localhost", "5432", "utente1", "47002", "ecommerce");

	ctx = redisConnect("localhost", 6379);

    unsigned int seed = init_rng_with_time();
    char user_stream_id[255] = {0};
    sprintf(user_stream_id, "%d", rand()); // random user for stream read

    while(1) {
        rep = readFromStream(ctx, (char *)"messages", user_stream_id, block, CUSTOMER_STREAM_CLIENT);
		for (int numStream = 0; numStream < getNumStreams(rep); numStream++) {
            char msg_id[255] = {0};
			for (int numMessage = 0; numMessage < getStreamLen(rep, numStream); numMessage++) { 
                readStreamMessageId(rep, numStream, numMessage, msg_id);
                char key[255] = {0};
                getStreamMessagesValues(rep, numStream, numMessage, 0, key); // the first key contains the command to be executed 
                if (DEBUG) printf("Command: %s\n", key);
                if(strcmp(key, "BUY_ITEM") == 0){
                    buyItem(ctx, rep, db, numStream, numMessage);
                    //break;
                } else if(strcmp(key, "LIST_OF_ITEMS") == 0) {
                    listOfItems(ctx, rep, db, numStream, numMessage);
                    //break;
                } else if(strcmp(key, "GET_CUSTOMER_ID") == 0) { 
                    getCustomerId(ctx, rep, db, numStream, numMessage);
                    //break;
                } else {
                    if (DEBUG) printf("Invalid command: %s\n", key);
                    //break;
                }
                ackMessage(ctx, CUSTOMER_STREAM_CLIENT, msg_id);
		    }
		}
        freeReplyObject(rep);
        if (DEBUG) printf("##############################\n");
	}
    redisFree(ctx);
    db.close_connection();
    return 0;
}