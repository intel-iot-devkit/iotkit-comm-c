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
* @file enableiot-subscriber.c
* @brief Sample to demonstrate data subscribe through iotkit-comm API.
*
* Provides features to connect and subscribe to data.
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm/iotkit-comm.h"
#include "iotkit-comm/util.h"

ServiceQuery *query = NULL;
CommHandle *commHandle = NULL;
int i = 0;

/**
 * @name Message handler
 * @brief Callback invoked upon receiving a message from enableiot cloud MQTT broker.
 * @param[in] message received from MQTT broker
 * @param[in] context with the topic information
 *
 * Callback invoked upon receiving a message from enableiot cloud MQTT broker.
 */
void message_callback(char *message, Context context) {
    printf("Message received:%s\n", message);

    i ++;

    if(i >= 3) {
        // clean the objects
        cleanUpService(&query, &commHandle);
        exit(0);
    }
}

int serviceStarted = 0;

/**
 * @name Callback to handle the communication
 * @brief Handles the communication with enableiot cloud MQTT broker once the connection is established.
 * @param[in] handle left for future purpose, currently unused
 * @param[in] error_code specifies the error code is any
 * @param[in] serviceHandle is the client object initialized with the required APIs
 *
 * Handles the communication with enableiot cloud MQTT broker once the connection is established.
 */
void callback(void *handle, int32_t error_code, void *serviceHandle) {
    if(serviceHandle != NULL && !serviceStarted) {
        commHandle = (CommHandle *) serviceHandle;
        char *response = NULL;
//        char * (**retrieve)(char *sensorName, char *deviceID, long long from, long long to) = NULL;
//        char * (**signIn)(char *username, char *passwd) = NULL;
        int (**receive)(void (*)(char *, Context)) = NULL;

        receive = commInterfacesLookup(commHandle, "receive");
        if(receive == NULL) {
            printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
            return;
        }

        /*retrieve = commInterfacesLookup(commHandle, "retrieve");
        if(retrieve == NULL) {
            printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
            return;
        }

        signIn = commInterfacesLookup(commHandle, "signIn");
        if(signIn == NULL) {
            printf("Function \'signIn\' is not available; please verify the Plugin documentation !!\n");
            return;
        }*/


        // read the user credentials from console
//        response = (*signIn)("username@server.com", "password");
//        response = (*retrieve)("garage_sensor", "02-00-86-4b-b6-de", 0, 0);
//        printf("Data Retrieved is: %s\n", response);
        (*receive)(message_callback);

        while(1) { // Infinite Event Loop
            sleep(1);
        }
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
 * @brief Starts the application to demonstrate subscription to an topic.
 *
 * Establishes the connection with an MQTT broker.
 */
int main(void) {

    puts("Sample program to test the IoT Cloud subscribe plugin !!");
    query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryEnableIot.json");

    if (query) {
        createClientForGivenService(query, callback);
    }

    return 0;
}
