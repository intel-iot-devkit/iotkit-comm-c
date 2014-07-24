/*
 * Distributed Thermostat sample program through Edison API
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

/** @file temperatureSensor.c

    Sample client program of temperature sensor based on ZMQ Pub/Sub
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <math.h>
#include "edisonapi.h"
#include "util.h"

/** Callback function. Once the service is advertised this callback function will be invoked

* @param servDesc the service description object
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
 */
void pubServiceCallback(ServiceDescription *servDesc, int32_t error_code, CommHandle *serviceHandle) {

    if (serviceHandle != NULL) {
        int (**publish)(char *,Context context);
        publish = commInterfacesLookup(serviceHandle, "publish");
        if (publish != NULL) {
            Context context;
            while(1) {
                char addr[128];
                double random = floor(rand() % 90 + 60);
                sprintf(addr, "mytemp: %f",random);
                (*publish)(addr,context);
                sleep(2);
            }
        } else {
            fprintf(stderr, "Interface lookup failed\n");
        }
    } else {
        fprintf(stderr, "Comm Handle is NULL\n");
    }
}

/** The starting point. Starts to advertise the given Service
*/
int main(void) {

    puts("Temperature sensor publishing its temperature data !!");
    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./serviceSpecs/temperature-sensor-spec.json");
    if (serviceDescription)
        WaitToAdvertiseService(serviceDescription, pubServiceCallback);
    return 0;
}
