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

/**
 * @file publisher.c
 * @brief Sample to demonstrate MQTT publisher through iotkit-comm API.
 *
 * Provides features to connect to an MQTT Broker and publish a topic.
 */

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

ServiceSpec *srvSpec = NULL;
int msgnumber = 0;

/**
 * @name Callback to handle the communication
 * @brief Handles the communication with an MQTT broker once the connection is established.
 * @param[in] handle left for future purpose, currently unused
 * @param[in] error_code specifies the error code is any
 * @param[in] serviceHandle is the client object initialized with the required APIs
 *
 * Handles the communication such as publishing data to an MQTT broker once the connection is established.
 */
void callback(void *handle, int32_t error_code, void *serviceHandle) {
    Context context;
    char msg[256];
    int i = 0;
    CommHandle *commHandle = NULL;

    if (serviceHandle != NULL) {
        commHandle = (CommHandle *)serviceHandle;

        int (**send)(char *, Context context);

        send = commInterfacesLookup(commHandle, "publish");
        if (send == NULL) {
            printf("Function \'send\' is not available; please verify the Plugin documentation !!\n");
            return;
        }

        context.name = "topic";
        context.value = "/foo";

        while(i < 10) {  // Event Loop
            sprintf(msg, "This is a test message %d", msgnumber++);
            printf("Publishing msg:%s\n", msg);

            (*send)(msg, context);
            sleep(2);

            i ++;
        }
    }

    // clean the objects
    cleanUpService(&srvSpec, &commHandle);
    exit(0);
}

/**
 * @name Starts the application
 * @brief Starts the application to publish for a topic.
 *
 * Establishes the connection with an MQTT broker.
 */
int main(void) {

    puts("Sample program to publish topic \'/foo\' !!");

    srvSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceMQTT.json");

    #if DEBUG
        printf("status:%d:service_name:%s:address:%s:port:%d:name:%s:protocol:%s\n", srvSpec->status, srvSpec->service_name, srvSpec->address, srvSpec->port, srvSpec->type.name, srvSpec->type.protocol);
    #endif

    if (srvSpec) {
        advertiseServiceBlocking(srvSpec, callback);
    }

    return 0;
}
