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

/** @file test_mdns_discoverservice_fail.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <stdlib.h>
#include <signal.h>
#include "iotkit-comm.h"


/** Callback function. Once the service is discovered this callback function will be invoked
* @param servQuery the service query object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subDiscoveryCallback(ServiceQuery *servQuery, int32_t error_code, CommHandle *commHandle) {
    puts("Fail: Service Not Found");
    exit(EXIT_SUCCESS);
}

int main(void) {
    ServiceQuery *servQuery = (ServiceQuery *) parseServiceQuery("./temperatureServiceQueryMQTT.json");
    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./temperatureServiceMQTT.json");
    /* Establish a handler for SIGALRM signals. */
    signal(SIGALRM, subDiscoveryCallback);
    /* Set an alarm to go off*/
    alarm(5);
    if (serviceSpec) {
        if (servQuery) {
            fprintf(stderr,"query host address %s\n",servQuery->address);
            fprintf(stderr,"query host port %d\n",servQuery->port);
            fprintf(stderr,"query service name %s\n",servQuery->service_name);
            printf("Waiting for the Service Upto 5 Seconds\n");
            discoverServicesBlocking(servQuery, subDiscoveryCallback);
        }
    }
    return 0;
}
