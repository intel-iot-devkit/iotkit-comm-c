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

/** @file thermostat.c

    Sample client program of thermostat controller based on ZMQ Pub/Sub
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

// maybe better to use a simple moving average, but then again,
// this is just a demo of how to use iotkit-comm
int cumulativeMovingAverage = 0;

// no. of temperature samples received from all sensors until now 	30
int sampleCount = 0;

// service instance to publish the latest mean temperature
int (**mypublisher)(char *,Context context);

/** Callback function. To to be invoked when it receives any messages for the subscribed topic
* @param message the message received from service/publisher
* @param context a context object
 */
void clientMessageCallback(char *message, Context context) {

    // remove the topic from the message content (the temperature itself)
    char *temperature = strstr(message,":");
    if (temperature != NULL) {
            temperature++;
    fprintf(stdout,"Received sample temperature %s\n",temperature);
    // compute the mean of the temperatures as they arrive
    double value = atof(temperature);
    sampleCount++;
    cumulativeMovingAverage = (value + sampleCount * cumulativeMovingAverage)/(sampleCount + 1);
    char addr[256];
    sprintf(addr, "%d", cumulativeMovingAverage);
    fprintf(stdout,"New average ambient temperature (cumulative) %s:\n",addr);
    char mean[256];
    sprintf(mean, "mean_temp: %s", addr);
    // the master (thermostat) publishes the average temperature so others
    // can subscribe to it.
    if (mypublisher != NULL)
        (*mypublisher)(mean,context);
    }
}

/** Callback function. Once the service is discovered this callback function will be invoked
* @param servQuery left for future purpose, currently unused
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subCallback(ServiceQuery *servQuery, int32_t error_code, CommHandle *commHandle) {

    int (**subscribe)(char *);
    int (**receive)(void (*)(char *, Context));
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/thermostat-spec.json");
    if (serviceSpec) {
        advertiseService(serviceSpec);
        CommHandle *serviceHandle = createService(NULL, serviceSpec);
        mypublisher = commInterfacesLookup(serviceHandle, "publish");
    }
    if (commHandle != NULL) {
        subscribe = commInterfacesLookup(commHandle, "subscribe");
        receive = commInterfacesLookup(commHandle, "receive");
        if (subscribe != NULL && receive != NULL) {
            while (1) {  // Infinite Event Loop
                (*subscribe)("mytemp");
                (*receive)(clientMessageCallback);
                sleep(2);
            }
        } else {
            fprintf(stderr, "Interface lookup failed\n");
        }
    } else {
        fprintf(stderr, "Comm Handle is NULL\n");
    }
}

/** The starting point. Starts browsing for the given Service name
 */
int main(void) {

    puts("Thermostat reading sensor data & publishing it to Dashboard");
    ServiceQuery *servQuery = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperature-sensor-query.json");
    if (servQuery) {
        discoverServicesBlocking(servQuery, subCallback);
    }
    return 0;
}
