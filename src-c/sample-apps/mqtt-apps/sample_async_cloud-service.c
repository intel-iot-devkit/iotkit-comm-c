#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

ServiceDescription *srvDesc = NULL;
void message_callback(char *message, Context context){
    printf("Message received:%s", message);
}


void callback(void *handle, int32_t error_code, ServiceDescription *desc)
{
Context context;
context.name = "topic";
context.value = "/foo";

    printf("message error=%d error_string=%s\nservice status=%d service name=%s\n",
	    error_code,
	    getLastError(),
	    desc ? desc->status : -1,
	    desc ? desc->service_name : "");

	    CommServiceHandle *commHandle;
	    commHandle = createService(srvDesc);

            commHandle->init("localhost", desc->port, "open", NULL);
            commHandle->setReceivedMessageHandler(message_callback);
//            commHandle->subscribe("/foo");

            commHandle->sendTo(NULL, "How are you", context);
}



int main(void) {

    void *handle;

	puts("Sample program to publish topic \'/foo\' !!");

    srvDesc = (ServiceDescription *) parseServiceDescription("../serviceSpecs/temperatureServiceMQTT.json");

    if (srvDesc)
	    handle = advertiseService(srvDesc, callback);


    while(1);
//	publish();

//	subscribe("/foo", NULL);

	//return EXIT_SUCCESS;
	return 0;
}

