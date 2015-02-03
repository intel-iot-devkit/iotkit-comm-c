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

/** @file test_zmqreqrep_req_socket_connect_fail.c

This file tests whether ZMQ Requester socket fails when we pass the Invalid IP Address.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "iotkit-comm.h"

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "127.0.0.123232132321323123232233";
        serviceQuery->port = 100;
        int result = init(serviceQuery, NULL);
        done();
        free(serviceQuery);
        if (result == 0) {
            puts("Requester Socket Successfully Connected");
        } else {
            puts("Failed: Requester Socket Binding");
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_FAILURE);
}
