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

/** @file dashboard.c

    Sample client program of dashboard based on ZMQ Pub/Sub
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

/** Callback function. To to be invoked when it receives any messages for the subscribed topic
* @param message the message received from service/publisher
* @param context a context object
 */
void clientMessageCallback(char *message, Context context) {

  // remove the topic from the message content (the temperature)
  char *temperature = strstr(message,":");
  if (temperature != NULL) {
        temperature++;
        fprintf(stdout,"Received mean temperature : %s\n", temperature);
  }
}

/** Callback function. Once the service is discovered this callback function will be invoked
* @param queryDesc left for future purpose, currently unused
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subDiscoveryCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

    int (**receive)(void (*)(char *, Context));
    if (commHandle != NULL) {
        receive = commInterfacesLookup(commHandle, "receive");
        if (receive != NULL) {
            while (1) { // Infinite Event Loop
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

    puts("Dashboard reading the mean temperature from thermostat");
    ServiceQuery *query = (ServiceQuery *) parseServiceQuery("./serviceQueries/thermostat-query.json");
    if (query) {
        discoverServicesBlocking(query, subDiscoveryCallback);
    }
    return 0;
}
