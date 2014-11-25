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

/** @file test_iotkitpubsub_client_publish_success.c

*/

#include <stdio.h>
#include <MQTTAsync.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "localhost";
        serviceQuery->port = 1884;
        int result = init(serviceQuery, NULL);
        if (result == MQTTASYNC_SUCCESS) {
            printf("Successfully Connected to an MQTT Broker\n");
            Context context;
            context.name = "topic";
            context.value = "data";
            result = send("{\"n\":\"garage5\",\"t\":\"temperature.v1.0\"}", context);
            if(result != MQTTASYNC_SUCCESS){
                printf("Test Failed: Could not register a sensor\n");
                exit(EXIT_FAILURE);
            }

            sleep(5); // wait for 5 seconds so that sensor registration completes hopefully

            result = send("{\"n\":\"garage5\",\"v\":185}", context);
            if(result == MQTTASYNC_SUCCESS){
                printf("Test Passed: Successfully published message\n");
                exit(EXIT_SUCCESS);
            }
        } else {
            printf("Test Failed: Could not connect to MQTT Broker\n");
        }
        done();
        free(serviceQuery);
    }
    exit(EXIT_FAILURE);
}
