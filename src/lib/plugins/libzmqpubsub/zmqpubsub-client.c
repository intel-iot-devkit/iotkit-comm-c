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

/** @file zmqpubsub-client.c
    Provides functions to subscribe on a topic, receive messages on a topic, unsubscribe from a topic.
*/

#include "zmqpubsub-client.h"

/** @defgroup zmqpubsubclient
*   This is ZMQ subscribe client.
*  @{
*/

/** Creates the context object and subscriber socket. With the help of the ServiceQuery parameter, the subscriber socket
establishes connection to the address and port to initiate communication.

* @param ClientServiceQuery an void pointer
* @param crypto authentication object
* @return The result code
*/
int init(void *ClientServiceQuery, Crypto *crypto) {
    #if DEBUG
        printf("In createClient\n");
    #endif
    ServiceQuery *serviceQuery = (ServiceQuery *)ClientServiceQuery;
    zmqContainer.context = zmq_ctx_new();
    #if DEBUG
        printf("context initialised\n");
    #endif
    char addr[128];
    sprintf(addr, "tcp://%s:%d\n", serviceQuery->address, serviceQuery->port);
    #if DEBUG
        printf("going to connect %s\n",addr);
    #endif
    zmqContainer.subscriber = zmq_socket(zmqContainer.context,ZMQ_SUB);
    int rc = zmq_connect(zmqContainer.subscriber,addr);
    #if DEBUG
        printf("subscriber connected\n");
    #endif

    #if DEBUG
        printf("Subscribing to %s\n", serviceQuery->service_name);
    #endif

    subscribe(serviceQuery->service_name);

    return rc;
}

int send(char *message,Context context) {
    #if DEBUG
        printf("\ninside sending message: %s\n",message);
    #endif
    return -1;
}

/** Subscribing to a topic. The client can subscribe to a topic in which he is interested to receive the messages.
* @param topic client is interested in
* @return The result code
*/
int subscribe(char *topic) {
    #if DEBUG
        printf("\nsubscribing for topic: %s\n",topic);
    #endif
    //  Subscribe only for the topic
    int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));
    return rc;
}

/** Receive the message. The parameter in this function is used as a callback mechanism to pass the
received message.
* @param subscribeClientHandler a callback handler
* @return The result code
*/
int receive(void (*subscribeClientHandler)(char *message, Context context)) {
    #if DEBUG
        printf("In receive Waiting for the message\n");
    #endif
    //  Read message contents
    char *contents = s_recv (zmqContainer.subscriber);
    #if DEBUG
        printf ("Message Received: %s\n", contents);
    #endif
    Context context;
    context.name = "event";
    context.value = "message";
    #if DEBUG
        printf ("calling handler\n");
    #endif
    subscribeClientHandler(contents,context);
    #if DEBUG
        printf ("\nfree the contents\n");
    #endif
    free (contents);
    return 0;
}

/** Unsubscribing to a topic. The client can unsubscribe, to stop receiving the messages on that topic.
* @param topic client wants to unsubscribe from
* @return The result code
*/
int unsubscribe(char *topic) {
    #if DEBUG
        printf("\nunsubscribing the topic: %s\n",topic);
    #endif
    int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_UNSUBSCRIBE, topic, strlen(topic));
    return rc;
}

/** Cleanup function. This function close the subscriber socket and destroy the
context object.
* @return The result code
*/
int done() {
    if (zmqContainer.subscriber != NULL) {
        zmq_close(zmqContainer.subscriber);
        zmqContainer.subscriber = NULL;
        #if DEBUG
            printf("subscriber freed\n");
        #endif
    }
    if (zmqContainer.context != NULL) {
        zmq_ctx_destroy(zmqContainer.context);
        zmqContainer.context = NULL;
        #if DEBUG
            printf("context freed\n");
        #endif
    }
    #if DEBUG
        printf("\nclosed\n");
    #endif
    return 0;
}

/** @} */ // end of zmqpubsubclient
