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

/** @file sample_zmqpubsub-client.c
    Sample client program of ZMQ subscriber.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm/iotkit-comm.h"
#include "iotkit-comm/util.h"

ServiceQuery *servQuery = NULL;

/** Callback function. To to be invoked when it receives any messages for the subscribed topic.
* @param message the message received from service/publisher
* @param context a context object
*/
void clientMessageCallback(char *message, Context context) {
    fprintf(stderr,"Message received in Client: %s\n", message);
}

/** Callback function. Once the service is discovered, this callback function will be invoked.
* @param[in] handle left for future purpose, currently unused
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
*/
void subDiscoveryCallback(void *handle, int32_t error_code, CommHandle *commHandle) {
    int (**receive)(void (*)(char *, Context));
    int i = 0, j = 0;

    if (commHandle != NULL) {
        receive = commInterfacesLookup(commHandle, "receive");

        if (receive != NULL) {
            while (j < 5) {  // Event Loop
                i++;
                if (i < 5) {
                    (*receive)(clientMessageCallback);
                }

                sleep(2);

                j ++;
            }

            // clean the service query object
            cleanUpService(&servQuery, &commHandle);
            exit(0);
        } else {
            puts("Interface lookup failed");
        }
    } else {
        puts("\nComm Handle is NULL\n");
    }
}

/** The starting point. Starts browsing for the given Service name.
*/
int main(void) {
    puts("Sample program to test the iotkit-comm ZMQ pub/sub plugin !!");
    servQuery = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryZMQPUBSUB.json");

    if (servQuery) {
        fprintf(stderr,"query host address %s\n",servQuery->address);
        fprintf(stderr,"query host port %d\n",servQuery->port);
        fprintf(stderr,"query service name %s\n",servQuery->service_name);
        discoverServicesBlocking(servQuery, subDiscoveryCallback);
    }

    return 0;
}
