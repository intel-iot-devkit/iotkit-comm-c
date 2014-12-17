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

/** @file sample_zmqreqrep-service.c
    Sample service program of ZMQ replier.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

ServiceSpec *serviceSpec = NULL;

/** Callback function. To to be invoked when it receives any messages from the Client.
* @param client the client object
* @param message the message received from client
* @param context a context object
*/
void repMessageCallback(void *client, char *message, Context context) {
    fprintf(stderr,"Message received in Server: %s\n", message);
}

/** Callback function. Once the service is advertised, this callback function will be invoked.
* @param[in] handle left for future purpose, currently unused
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
*/
void repAdvertiseCallback(void *handle, int32_t error_code,CommHandle *serviceHandle) {
    if (serviceHandle != NULL) {
        void *client = NULL;
        Context context;
        void (**sendTo)(void *, char *, Context context);
        int (**receive)(void (*)(void *, char *, Context context));
        int i = 0;

        sendTo = commInterfacesLookup(serviceHandle, "sendTo");
        receive = commInterfacesLookup(serviceHandle, "receive");
        if (sendTo != NULL && receive != NULL) {
            while(i < 10) {  // Event Loop
                (*sendTo)(client,"train bike car",context);
                (*receive)(repMessageCallback);
                sleep(2);

                i ++;
            }

            // clean the service specification object
            cleanUpService(&serviceSpec, &serviceHandle);
            exit(0);
        } else {
            puts("Interface lookup failed");
        }
    } else {
        puts("\nComm Handle is NULL\n");
    }
}

/** The starting point. Starts to advertise the given Service.
*/
int main(void) {
    puts("Sample program to test the iotkit-comm ZMQ req/rep plugin !!");
    serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceZMQREQREP.json");

    if (serviceSpec) {
        advertiseServiceBlocking(serviceSpec, repAdvertiseCallback);
    }

    return 0;
}
