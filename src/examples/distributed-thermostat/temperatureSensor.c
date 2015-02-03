/*
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** @file temperatureSensor.c

    Sample client program of temperature sensor based on ZMQ Pub/Sub
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <math.h>
#include "iotkit-comm/iotkit-comm.h"
#include "iotkit-comm/util.h"

char *topic = NULL;

/** Callback function. Once the service is advertised this callback function will be invoked

* @param servSpec left for future purpose, currently unused
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
 */
void pubServiceCallback(ServiceSpec *servSpec, int32_t error_code, CommHandle *serviceHandle) {

    if (serviceHandle != NULL) {
        int (**publish)(char *,Context context);
        publish = commInterfacesLookup(serviceHandle, "publish");
        if (publish != NULL) {
            Context context;
            while(1) {  // Infinite Event Loop
                char addr[128];
                double random = floor(rand() % 90 + 60);
                sprintf(addr, "%s: %f",topic, random);
                printf("Publishing ... %s\n", addr);
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
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperature-sensor-spec.json");
    if (serviceSpec) {
        topic = serviceSpec->service_name;
        advertiseServiceBlocking(serviceSpec, pubServiceCallback);
    }

    return 0;
}
