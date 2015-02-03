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

/** @file test_zmqreqrep_req_send_fail.c

This file tests whether ZMQ Requester socket fails while sending message.
*/

#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "iotkit-comm.h"

void handler(char *message,Context context) {
    printf("Received message: %s\n",message);
}

int main(void) {
    ServiceQuery *serviceQuery = (ServiceQuery *)malloc(sizeof(ServiceQuery));
    if (serviceQuery != NULL) {
        serviceQuery->address = "127.0.0.1";
        serviceQuery->port = 5560;
        int result = init(serviceQuery, NULL);
        if (result == -1)
            puts("Requester init failed");
        void *ctx = zmq_ctx_new();
        assert (ctx);
        void *pub = zmq_socket(ctx, ZMQ_REP);
        assert (pub);
        int rc = zmq_bind(pub, "tcp://127.0.0.1:5560");
        assert (rc == 0);
        result = send("apple",NULL);
        if (result == -1) {
            puts("send failed");
            rc = zmq_close(pub);
            assert(rc == 0);
            rc = zmq_ctx_term(ctx);
            assert(rc == 0);
            free(serviceQuery);
        } else {
            puts("Requester Sent Message Successfully");
            rc = zmq_close(pub);
            assert (rc == 0);
            rc = zmq_ctx_term(ctx);
            assert(rc == 0);
            free(serviceQuery);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_FAILURE);
}
