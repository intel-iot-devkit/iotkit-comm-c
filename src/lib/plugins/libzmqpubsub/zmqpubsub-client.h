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

/** @file zmqpubsub-client.h
    Header file. List of all functions in zmq subscribe client.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../inc/zhelpers.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

#include "iotkit-comm.h"

/** @defgroup zmqpubsubclient
*
*  @{
*/

char *interface = "client-interface"; /**< specifies the plugin interface json */
bool provides_secure_comm = false;

/** Structure holds the context and subscriber handler
*/
struct ZMQPubSubClient {
    void *context; /**< context handler */
    void *subscriber; /**< subscriber handler */
};

/** An Global ZMQPubSubClient Object.
*/
struct ZMQPubSubClient zmqContainer;

int init(void *ClientServiceQuery, Crypto *crypto);
int send(char *message,Context context);
int subscribe(char *topic);
int unsubscribe(char *topic);
int receive(void (*subscribeClientHandler)(char *message, Context context));
int done();

/** @} */ // end of zmqpubsubclient
