#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

ServiceQuery *query = NULL;
void message_callback(char *message, Context context) {
    fprintf(stderr,"Message received in Client: %s\n", message);
}


void callback(void *handle, int32_t error_code, CommClientHandle *commHandle)
{
        if (commHandle != NULL) {
        Context context;
        while (1) {
            commHandle->send("toys",context);
            commHandle->receive(message_callback);
            sleep(2);
        }
        } else {
            puts("\nComm Handle is NULL\n");
        }

}

int main(void) {

    void *handle;

	puts("Sample program to test the Edison ZMQ req/rep plugin !!");
    query = (ServiceQuery *) parseServiceDescription("./serviceSpecs/temperatureServiceZMQREQREP.json");

    if (query) {
        fprintf(stderr,"query host address %s\n",query->address);
        fprintf(stderr,"query host port %d\n",query->port);
        fprintf(stderr,"query service name %s\n",query->service_name);
	    handle = (void *) discoverServices(query, callback);
	}


    while(1);

	return 0;
}

