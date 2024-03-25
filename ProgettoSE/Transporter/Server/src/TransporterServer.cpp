#include "TransporterServer.h"

int main() {    
    setvbuf(stdout, (char*) NULL, _IONBF, 0);
    setvbuf(stderr, (char*) NULL, _IONBF, 0);
	if (DEBUG) printf("Hello, transporter server!\n");

    redisContext *ctx; 
    redisReply *rep; 
    int block = 100000000; 

    Con2DB ecommerce("localhost", "5432", "utente1", "47002", "ecommerce");

	ctx = redisConnect("localhost", 6379);

    unsigned int seed = init_rng_with_time();
    char user_stream_id[255] = {0};
    sprintf(user_stream_id, "%d", rand());

    while(1){
        rep = readFromStream(ctx, (char *)"messages", user_stream_id, block, TRANSPORTER_STREAM_CLIENT);
        dumpReply(rep, 0);
        for(int numStream = 0; numStream < getNumStreams(rep); numStream++){
            char msg_id[255] = {0};
            for(int numMessage = 0; numMessage < getStreamLen(rep, numStream); numMessage++){
                readStreamMessageId(rep, numStream, numMessage, msg_id);
                char key[255] = {0};
                getStreamMessagesValues(rep, numStream, numMessage, 0, key);
                if (DEBUG) printf("Command: %s\n", key);
                if(strcmp(key, "GET_TRANSPORTER_ID") == 0){
                    getTransporterId(ctx, rep, ecommerce, numStream, numMessage);
                    //break;
                } else if(strcmp(key, "TAKE_ORDER") == 0){
                    takeOrder(ctx, rep, ecommerce, numStream, numMessage);
                    //break;
                } else if(strcmp(key, "DELIVER_ORDER") == 0){
                    deliverOrder(ctx, rep, ecommerce, numStream, numMessage);
                    //break;
                } else if(strcmp(key, "HAS_ORDERS") == 0){
                    hasOrders(ctx, rep, ecommerce, numStream, numMessage);
                    //break;
                } else {
                    if (DEBUG) printf("Unknown command: %s\n", key);
                    //break;
                }
                ackMessage(ctx, FURNISHER_STREAM_CLIENT, msg_id);
            }
        }
        freeReplyObject(rep);
        if (DEBUG) printf("####################\n");
    }
    redisFree(ctx);
    ecommerce.close_connection();
    return 0;
}
