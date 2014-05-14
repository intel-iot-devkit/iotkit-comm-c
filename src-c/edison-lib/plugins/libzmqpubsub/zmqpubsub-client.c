/*
 * ZMQ PUB/SUB plugin to enable subscribe feature through Edison API
 * Copyright (c) 2014, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 */

/** @file zmqpubsub-client.c

    This class provides functions to subscribe on a topic,to receive messages on a topic,to
 unsubscribe from a topic.
 */

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "zmqpubsub-client.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 0
#endif

/** @defgroup zmqpubsubclient
*   This is ZMQ PUB/SUB Client
*  @{
*/

/** Structure which holds the context and subscriber handler
 */
struct ZMQPubSubClient {
	void *context; /**< context handler */
	void *subscriber; /**< subscriber handler */
};

/** An Global ZMQPubSubClient Object.
 */
struct ZMQPubSubClient zmqContainer;

/** Creates the context object and subscriber socket. With the help of the ServiceQuery parameter the subscriber socket
establishes connection to the address and port to initiate communication.

* @param ClientServiceQuery an void pointer
* @return The result code
*/
int init(void *ClientServiceQuery) {
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
    return rc;
}

/** Empty function. This function is unimplemented since in ZMQ pub/sub client we use only
subscribe and receive
* @param message a string message
* @param context a context object
* @return The result code

*/
int send(char *message,Context context) {
    #if DEBUG
    	printf("\ninside sending message: %s\n",message);
    #endif
}

/** Subscribing for a topic. The client can subscribe for a topic in which he is interested
in to receive the messages.

* @param topic which client is interested in
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

/** Receive the message. The client will be passing an handler which is used as a callback mechanism to pass the
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
    handler(contents,context);
    #if DEBUG
        printf ("\nfree the contents\n");
    #endif
    free (contents);

}

/** Unsubscribing a topic. The client can unsubscribe a topic in which he is no more
interested to receive the messages.
* @param topic which client wants to unsubscribe from
* @return The result code

*/
int unsubscribe(char *topic) {
	#if DEBUG
		printf("\nunsubscribing the topic: %s\n",topic);
	#endif
    int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_UNSUBSCRIBE, topic, strlen(topic));
    return rc;
}

/** Cleanup function. This method helps in closing the subscriber socket and destroying the
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
}

/** @} */ // end of zmqpubsubclient