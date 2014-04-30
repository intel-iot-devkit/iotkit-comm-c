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

#ifndef DEBUG
    #define DEBUG 1
#endif

ServiceDescription *srvDesc = NULL;
int msgnumber = 0;

void callback(void *handle, int32_t error_code, void *serviceHandle)
{
    Context context;
    char msg[256];

    if(serviceHandle != NULL){
        CommServiceHandle *commHandle = (CommServiceHandle *)serviceHandle;

        context.name = "topic";
        context.value = "/foo";
    //    commHandle->init("localhost", srvDesc->port, "open", NULL);

        while(1){
            sprintf(msg, "This is a test message %d", msgnumber++);
            commHandle->sendTo(NULL, msg, context);
            sleep(2);
        }
    }
}


int main(void) {

    void *handle;

	puts("Sample program to publish topic \'/foo\' !!");

    srvDesc = (ServiceDescription *) parseServiceDescription("../serviceSpecs/temperatureServiceMQTT.json");

    printf("111status:%d:service_name:%s:address:%s:port:%d:name:%s:protocol:%s\n", srvDesc->status, srvDesc->service_name, srvDesc->address, srvDesc->port, srvDesc->type.name, srvDesc->type.protocol);


    if(srvDesc)
	    advertiseService(srvDesc, callback);

	//return EXIT_SUCCESS;
	return 0;
}

