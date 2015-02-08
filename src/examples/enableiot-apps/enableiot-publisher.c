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
* @file enableiot-publisher.c
* @brief Sample to demonstrate data publish through iotkit-comm API.
*
* Provides features to publish data to enableiot cloud
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm/iotkit-comm.h"
#include "iotkit-comm/util.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

ServiceSpec *srvSpec = NULL;
int msgnumber = 13; // iotkit-agent does not accept zero as sensor value; so assigning a non-zero value

/**
 * @name Callback to handle the communication
 * @brief Handles the communication with enableiot cloud once the connection is established.
 * @param[in] handle left for future purpose, currently unused
 * @param[in] error_code specifies the error code any
 * @param[in] serviceHandle is the client object initialized with the required APIs
 *
 * Handles the communication, such as publishing data once the connection is established.
 */
void callback(void *handle, int32_t error_code, void *serviceHandle) {
    Context context;
    char msg[256];
    int i = 0;
    CommHandle *commHandle = NULL;

    if(serviceHandle != NULL) {
        commHandle = (CommHandle *) serviceHandle;

        int (**publish) (char *message,Context context);

        publish = commInterfacesLookup(commHandle, "publish");
        if(publish == NULL) {
            fprintf(stderr, "Function \'publish\' is not available; please verify the Plugin documentation !!\n");
            return;
        }

        context.name = "topic";
        context.value = "data";

        while(i < 10) {  // Event Loop
            sprintf(msg, "{\"name\": \"garage_sensor\", \"value\": %d}", msgnumber++);
            printf("Publishing msg:%s\n", msg);

            (*publish)(msg, context);
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
 * @brief Starts the application to publish data
 *
 * Establishes the connection with enableiot cloud
 */
int main(void) {
    puts("Sample program to publish data to IoT Cloud !!");

    srvSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceEnableIot.json");

    if (srvSpec){
        advertiseServiceBlocking(srvSpec, callback);
    }

    return 0;
}
