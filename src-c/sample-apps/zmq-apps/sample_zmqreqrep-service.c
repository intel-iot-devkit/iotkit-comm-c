/*
 * ZMQ REQ/REP sample program through Edison API
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

/** @file sample_zmqreqrep-service.c

    Sample service program of ZMQ REQ/REP

 */

#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

/** Callback function. To to be invoked when it receives any messages from the Client
* @param client the client object
* @param message the message received from client
* @param context a context object
 */
void repMessageCallback(void *client, char *message, Context context) {
    fprintf(stderr,"Message received in Server: %s\n", message);
}

/** Callback function. Once the service is advertised this callback function will be invoked

* @param servDesc the service description object
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
 */
void repAdvertiseCallback(ServiceDescription *servDesc, int32_t error_code,CommHandle *serviceHandle)
{
        if (serviceHandle != NULL) {
	    void *client;
	    Context context;
	    void (**sendTo)(void *, char *, Context context);
	    int (**receive)(void (*)(void *, char *, Context context));

        sendTo = commInterfacesLookup(serviceHandle, "sendTo"); //(int (*)(void *, char *, Context))
        receive = commInterfacesLookup(serviceHandle, "receive"); //(int (*)(void *, char *, Context))

	    while(1) {
            (*sendTo)(client,"train bike car",context);
            (*receive)(repMessageCallback);
            sleep(2);
        }
        } else {
                    puts("\nComm Handle is NULL\n");
                }
}


/** The starting point. Starts to advertise the given Service
*/
int main(void) {

	puts("Sample program to test the Edison ZMQ req/rep plugin !!");
    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./serviceSpecs/temperatureServiceZMQREQREP.json");

    if (serviceDescription)
	    WaitToAdvertiseService(serviceDescription, repAdvertiseCallback);

	return 0;
}

