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
* @file subscriber.c
* @brief Sample to demonstrate MQTT subscriber through iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and subscribe to a topic.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm.h"
#include "util.h"

ServiceQuery *query = NULL;
CommHandle *commHandle = NULL;
int i = 0;

/**
 * @name Message handler
 * @brief Callback invoked upon receiving a message from an MQTT broker.
 * @param[in] message received from an MQTT broker
 * @param[in] context with the topic information
 *
 * Callback invoked upon receiving a message from an MQTT broker.
 */
void message_callback(char *message, Context context) {
    printf("Message received:%s\n", message);

    i ++;

    if(i >= 5) {
        // clean the service query object
        cleanUpService(&query, &commHandle);
        exit(0);
    }
}

int serviceStarted = 0;

/**
 * @name Callback to handle the communication
 * @brief Handles the communication with an MQTT broker once the connection is established.
 * @param[in] handle left for future purpose, currently unused
 * @param[in] error_code specifies the error code is any
 * @param[in] serviceHandle is the client object initialized with the required APIs
 *
 * Handles the communication with an MQTT broker once the connection is established.
 */
void callback(void *handle, int32_t error_code, void *serviceHandle) {
    if (serviceHandle != NULL && !serviceStarted) {
        commHandle = (CommHandle *) serviceHandle;
        int (**receive)(void (*)(char *, Context)) = NULL;

        receive = commInterfacesLookup(commHandle, "receive");
        if (receive == NULL) {
            printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
            return;
        }

        (*receive)(message_callback);

        serviceStarted = 1;
    }
}

/**
 * @name Callback to filter discovered services
 * @brief Handles service filtering after discovery
 * @param[in] srvQuery service query object
 *
 * Handles service filtering after discovery
 */
bool serviceFilter(ServiceQuery *srvQuery) {
    printf("Got into Service Filter\n");
    return true;
}

/**
 * @name Starts the application
 * @brief Starts the application to subscribe to a topic.
 *
 * Establishes the connection with an MQTT broker.
 */
int main(void) {

    puts("Sample program to test the iotkit-comm MQTT pub/sub plugin !!");
    query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryMQTT.json");

    if (query) {
        discoverServicesBlockingFiltered(query, serviceFilter, callback);
    }

    return 0;
}
