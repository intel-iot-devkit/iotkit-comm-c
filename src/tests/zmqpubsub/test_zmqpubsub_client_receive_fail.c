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

/** @file test_zmqpubsub_client_subscribe_success.c

 */

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <signal.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"

void handler() {
    puts("Didn't receive message");
    exit(EXIT_SUCCESS);
}

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->service_name = strdup("/foo");
        serviceQuery->address = strdup("localhost");
        serviceQuery->port = 5563;
        int result = init(serviceQuery, NULL);
        if (result == -1)
            puts("client init failed");
        result = subscribe("flower");
        if (result == -1)
            puts("client subscribe failed");
        /* Establish a handler for SIGALRM signals. */
        signal(SIGALRM, handler);
        /* Set an alarm to go off*/
        alarm(3);
        result = receive(handler);
        if (result == -1)
            puts("receive failed");
        done();
        free(serviceQuery);
    }
    exit(EXIT_FAILURE);
}
