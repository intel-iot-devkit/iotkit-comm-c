#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

ServiceQuery *query = NULL;
void message_callback(char *message, Context context){
    printf("Message received:%s", message);
}


void callback(void *handle, int32_t error_code, ServiceDescription *desc)
{
    printf("message error=%d error_string=%s\nservice status=%d service name=%s\n",
	    error_code,
	    getLastError(),
	    desc ? desc->status : -1,
	    desc ? desc->service_name : "");

	    CommClientHandle *commHandle;
	    commHandle = createClient(query);

            commHandle->setReceivedMessageHandler(message_callback);
            commHandle->subscribe("/foo");
}



int main(void) {

    void *handle;

	puts("Sample program to test the Edison MQTT pub/sub plugin !!");

    query = (ServiceQuery *) parseServiceDescription("../serviceSpecs/temperatureServiceMQTT.json");

    if (query)
	    handle = discoverServices(query, callback);


    while(1);
//	publish();

//	subscribe("/foo", NULL);

	//return EXIT_SUCCESS;
	return 0;
}

