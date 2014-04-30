#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

ServiceDescription *serviceDescription = NULL;

void message_callback(void *client, char *message, Context context) {
    fprintf(stderr,"Message received in Server: %s\n", message);
}

void callback(void *handle, int32_t error_code,CommServiceHandle *serviceHandle)
{
        if (serviceHandle != NULL) {
	    void *client;
	    Context context;
	    while(1) {
            serviceHandle->sendTo(client,"train bike car",context);
            serviceHandle->receive(message_callback);
            sleep(2);
        }
        } else {
                    puts("\nComm Handle is NULL\n");
                }
}



int main(void) {

    void *handle;
	puts("Sample program to test the Edison ZMQ req/rep plugin !!");
    serviceDescription = (ServiceDescription *) parseServiceDescription("./serviceSpecs/temperatureServiceZMQREQREP.json");

    if (serviceDescription)
	    handle = advertiseService(serviceDescription, callback);


    while(1);

	return 0;
}

