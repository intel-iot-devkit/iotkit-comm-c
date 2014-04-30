/*
 * Sample program to demonstrate MQTT Async subscribe feature through Edison API
 * Copyright (c) 2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 */

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


void callback(void *handle, int32_t error_code, void *serviceHandle)
{
    if(serviceHandle != NULL){
        CommClientHandle *commHandle = (CommClientHandle *) serviceHandle;

    //    commHandle->init("localhost", query->port, "open", NULL);
        commHandle->subscribe("/foo");
        commHandle->receive(message_callback);
    }
}



int main(void) {

    void *handle;

	puts("Sample program to test the Edison MQTT pub/sub plugin !!");

    query = (ServiceQuery *) parseServiceDescription("../serviceSpecs/temperatureServiceMQTT.json");

    if (query)
	    discoverServices(query, callback);


//    while(1);
//	publish();

//	subscribe("/foo", NULL);

	//return EXIT_SUCCESS;
	return 0;
}

