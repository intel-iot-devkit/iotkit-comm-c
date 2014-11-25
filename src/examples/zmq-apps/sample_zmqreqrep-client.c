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

/** @file sample_zmqreqrep-client.c
    Sample client program of ZMQ requester.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

ServiceQuery *query = NULL;

/** Callback function. To be invoked when it receives any messages from the Service.
* @param message the message received from service
* @param context a context object
*/
void reqMessageCallback(char *message, Context context) {
    fprintf(stderr,"Message received in Client: %s\n", message);
}

/** Callback function. Once the service is discovered, this callback function will be invoked.

* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
*/
void reqDiscoveryCallback(void *handle, int32_t error_code, CommHandle *commHandle) {
    if (commHandle != NULL) {
        int (**send)(char *, Context context);
        int (**receive)(void (*)(char *, Context));
        Context context;
        int i = 0;

        send = commInterfacesLookup(commHandle, "send");
        receive = commInterfacesLookup(commHandle, "receive");
        if (send != NULL && receive != NULL) {
            while (i < 9) { // Event Loop
                (*send)("toys",context);
                (*receive)(reqMessageCallback);
                sleep(2);

                i ++;
            }

            // clean the service query object
            cleanUpService(&query, &commHandle);
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
    puts("Sample program to test the iotkit-comm ZMQ req/rep plugin !!");
    query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryZMQREQREP.json");

    if (query) {
        fprintf(stderr,"query host address %s\n",query->address);
        fprintf(stderr,"query host port %d\n",query->port);
        fprintf(stderr,"query service name %s\n",query->service_name);
        discoverServicesBlocking(query, reqDiscoveryCallback);
    }

    return 0;
}
