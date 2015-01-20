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

/** @file zmqreqrep-client.h
    Header file. Lists all those functions in implementing ZMQ requester.
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>

#include "iotkit-comm.h"

#ifndef DEBUG
    #define DEBUG 1
#endif

/** @defgroup zmqreqrepclient
*
*  @{
*/

char *interface = "client-interface"; /**< specifies the plugin interface json */
bool provides_secure_comm = false;

/** Structure holds the context and requester handler
*/
struct ZMQReqRepClient {
    void *context; /**< context handler */
    void *requester; /**< requester handler */
};

/** An Global ZMQReqRepClient Object.
*/
struct ZMQReqRepClient zmqContainer;

int init(void *requestClientQuery, Crypto *crypto);
int send(char *message, Context);
int receive(void (*requestClientHandler)(char *message, Context context));
int done();
/** @} */ // end of zmqreqrepclient
