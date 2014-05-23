/*
 * ZMQ PUB/SUB sample program through Edison API
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

    Sample service program of ZMQ PUB/SUB

 */

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iecfapi/edisonapi.h"
#include "iecfapi/util.h"

/** Callback function. Once the service is advertised this callback function will be invoked

* @param servDesc the service description object
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
 */
void pubServiceCallback(ServiceDescription *servDesc, int32_t error_code, CommHandle *serviceHandle)
{
	    if (serviceHandle != NULL) {

	    int (**publish)(char *,Context context);

        publish = commInterfacesLookup(serviceHandle, "publish");

        Context context;
        while(1) {
            (*publish)("vehicle: car",context);
            sleep(2);
        }
        } else {
            puts("\nComm Handle is NULL\n");
        }

}


/** The starting point. Starts to advertise the given Service
*/
int main(void) {

	puts("Sample program to test the Edison ZMQ pub/sub plugin !!");
    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

    if (serviceDescription)
	    WaitToAdvertiseService(serviceDescription, pubServiceCallback);

	return 0;
}

