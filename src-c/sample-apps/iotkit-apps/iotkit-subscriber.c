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

/**
 * @file sample_async_cloud-client.c
 * @brief Sample to demonstrate MQTT subscriber through Edison API
 *
 * Provides features to connect to an MQTT Broker and subscribe to a topic
 */

#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

ServiceQuery *query = NULL;

/**
 * @name message handler
 * @brief callback invoked upon receiving a message from an MQTT broker
 * @param[in] message received from an MQTT broker
 * @param[in] context with the topic information
 *
 * callback invoked upon receiving a message from an MQTT broker
 */
void message_callback(char *message, Context context){
    printf("Message received:%s\n", message);
}

// TODO: MDNS does frequent add/rem operations due to which multiple clients get created
int serviceStarted = 0; // temporary fix to avoid recreation of client due to frequent MDNS add/rem operations

/**
 * @name callback to handle the communication
 * @brief handles the communication with an MQTT broker once the connection is established
 * @param[in] error_code specifies the error code is any
 * @param[in] serviceHandle is the client object initialized with the required APIs
 *
 * handles the communication with an MQTT broker once the connection is established
 */
void callback(void *handle, int32_t error_code, void *serviceHandle)
{
    if(serviceHandle != NULL && !serviceStarted){
        CommHandle *commHandle = (CommHandle *) serviceHandle;
        int (**subscribe)() = NULL;
        int (**receive)(void (*)(char *, Context)) = NULL;

        subscribe = commInterfacesLookup(commHandle, "subscribe");
        if(subscribe == NULL){
            printf("Function \'subscribe\' is not available; please verify the Plugin documentation !!\n");
        }

        receive = commInterfacesLookup(commHandle, "receive"); //(int (*)(void (*)(char *, Context)))
        if(receive == NULL){
            printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
        }

        (*receive)(message_callback);
        (*subscribe)();

        serviceStarted = 1;
    }
}

bool serviceFilter(ServiceQuery *srvQuery){

    printf("Got into Service Filter\n");
    return true;
}

/**
 * @name Starts the application
 * @brief Starts the application to demonstrate subscription to an topic
 *
 * Establishes the connection with an MQTT broker
 */
int main(void) {

	puts("Sample program to test the Edison IoT Cloud subscribe plugin !!");
    query = (ServiceQuery *) parseServiceDescription("./serviceSpecs/temperatureServiceIoTKit.json");

    if (query){
        createClientForGivenService(query, callback);
    }
//	    WaitToDiscoverServicesFiltered(query, serviceFilter, callback);

	return 0;
}

