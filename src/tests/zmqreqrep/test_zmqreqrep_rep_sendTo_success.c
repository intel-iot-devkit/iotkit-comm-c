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

/** @file test_zmqreqrep_rep_sendTo_fail.c

This file tests whether ZMQ Responder socket fails while sending message.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "iotkit-comm.h"

void handler(void *client,char *message,Context context) {
    printf("Received message: %s\n",message);
}

int main(void) {
    ServiceSpec *serviceSpec = (ServiceSpec *)malloc(sizeof(ServiceSpec));
    if (serviceSpec != NULL) {
        serviceSpec->address = "127.0.0.1";
        serviceSpec->port = 1234;
        init(serviceSpec, NULL);
        void *ctx = zmq_ctx_new();
        void *req = zmq_socket(ctx, ZMQ_REQ);
        int result = zmq_connect(req, "tcp://127.0.0.1:1234");
        if (result == -1)
            puts("request connect failed");
        //  Send message from client to server
        int rc = zmq_send(req, "rose", 4, 0);
        if (rc == -1)
            puts("client send failed");
        puts("waitng for message");
        receive(handler);
        // send message from server to client
        result = sendTo(NULL,"Hello World",NULL);
        if (result == -1)
            puts("sendTo failed");

        //  Receive message at client side
        char buffer [12];
        rc = zmq_recv(req, buffer, 11, 0);
        if (rc == -1) {
            puts("receive failed");
            zmq_close(req);
            zmq_ctx_term(ctx);
        } else {
            buffer[rc] = '\0';
            printf("message received is %s\n",buffer);
            zmq_close(req);
            zmq_ctx_term(ctx);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_FAILURE);
}
