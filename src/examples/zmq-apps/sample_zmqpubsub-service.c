/*
 * ZMQ PUB/SUB sample program through iotkit-comm API
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

/** @file sample_zmqpubsub-service.c
    Sample service program of ZMQ publisher.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

ServiceSpec *serviceSpec = NULL;

/** Callback function. Once the service is advertised, this callback function will be invoked.
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
*/
void pubServiceCallback(void *handle, int32_t error_code, CommHandle *serviceHandle) {
    if (serviceHandle != NULL) {
        int (**publish)(char *,Context context);
        int i = 0;

        publish = commInterfacesLookup(serviceHandle, "publish");
        if (publish != NULL) {
            Context context;
            while(i < 10) { // Event Loop
                (*publish)("vehicle: car",context);
                sleep(2);

                i ++;
            }

            // clean the service specification object
            cleanUpService(serviceSpec);
            exit(0);
        } {
            puts("Interface lookup failed");
        }
    } else {
        puts("\nComm Handle is NULL\n");
    }
}

/** The starting point. Starts to advertise the given Service.
*/
int main(void) {
    puts("Sample program to test the iotkit-comm ZMQ pub/sub plugin !!");
    serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

    if (serviceSpec) {
        advertiseServiceBlocking(serviceSpec, pubServiceCallback);
    }

    return 0;
}
