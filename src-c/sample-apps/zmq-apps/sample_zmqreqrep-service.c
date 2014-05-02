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

/** a callback function to be invoked when it receives any messages from the client
 */
void message_callback(void *client, char *message, Context context) {
    fprintf(stderr,"Message received in Server: %s\n", message);
}

/** a callback function to be invoked when the service is advertised
 */
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


/** The starting point of sample program
 */
int main(void) {

    void *handle;
	puts("Sample program to test the Edison ZMQ req/rep plugin !!");
    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("../serviceSpecs/temperatureServiceZMQREQREP.json");

    if (serviceDescription)
	    handle = advertiseService(serviceDescription, callback);

	return 0;
}

