/*
 * ZMQ REQ/REP plugin to enable request feature through Edison API
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

/** @file zmqreqrep-client.c

    This class provides functions to send and receive message to/from a Service to which it
    is connected to.

 */
#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include "zmqreqrep-client.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 0
#endif

/** @defgroup zmqreqrepclient
*  This is ZMQ REQ REP Client

*  @{

*/

char *interface = "edison-client-interface"; /**< specifies the plugin interface json */


/** Structure which holds the context and requester handler
 */
struct ZMQReqRepClient {
	void *context; /**< context handler */
	void *requester; /**< requester handler */
};

/** An Global ZMQReqRepClient Object.
 */
struct ZMQReqRepClient zmqContainer;

/** Creates the context object and requester socket. With the help of the ServiceQuery parameter the requester socket
establishes connection to the address and port to initiate communication.

* @param requestClientQuery an void pointer
* @return The result code
*/
int init(void *requestClientQuery) {
	#if DEBUG
		printf("context initialised\n");
	#endif
	ServiceQuery *serviceQuery = (ServiceQuery *)requestClientQuery;
	zmqContainer.context = zmq_ctx_new();
    int rc = -1;
    // This is client side
    zmqContainer.requester = zmq_socket(zmqContainer.context,ZMQ_REQ);
    char addr[128];
    sprintf(addr, "tcp://%s:%d",serviceQuery->address, serviceQuery->port);
    #if DEBUG
        printf("going to connect %s\n",addr);
    #endif
    rc = zmq_connect(zmqContainer.requester,addr);
    #if DEBUG
        printf("connect completed\n");
    #endif
	return rc;
}

/** Sending a message. The client can send a message to the service to which it is connected to.

* @param message a string message
* @param context a context message
* @return The result code

*/
int send(char *message,Context context) {
	#if DEBUG
    	printf ("Sending message from Client %s...\n",message);
	#endif
	int rc = s_send(zmqContainer.requester, message);
	return rc;
}

/**  Empty function. This function is unimplemented since in ZMQ req/rep this is not applicable

* @param topic a string message
* @return The result code
*/
int subscribe(char *topic) {
	#if DEBUG
		printf("In subscribe\n");
	#endif
}

/**  Empty function. This function is unimplemented since in ZMQ req/rep this is not applicable

* @param topic a string message
* @return The result code
*/
int unsubscribe(char *topic) {
	#if DEBUG
		printf("In unsubscribe\n");
	#endif
}

/** Receive the message. The client will be passing an handler which is used as a callback mechanism to pass the
received message.
* @param requestClientHandler a callback handler which takes message and context object as params
* @return The result code

*/
int receive(void (*requestClientHandler)(char *message, Context context)) {
	#if DEBUG
		printf("In receive\n");
	#endif
	int rc = 0;
	//  Read message contents
    char *contents = s_recv (zmqContainer.requester);
    #if DEBUG
        printf ("Received message in client: %s\n", contents);
    #endif
    if (contents == NULL) {
        rc = -1;
    }
    Context context;
    context.name = "event";
    context.value = "message";
    requestClientHandler(contents,context);
    free(contents);
}

/** Cleanup function. This method helps in closing the requester socket and destroying the
context object.
* @return The result code
*/
int done() {
    int rc = -1;
	if (zmqContainer.requester != NULL) {
		rc = zmq_close(zmqContainer.requester);
		zmqContainer.requester = NULL;
		#if DEBUG
			printf("requester freed\n");
		#endif
	}
	if (zmqContainer.context != NULL) {
		rc = zmq_ctx_destroy(zmqContainer.context);
		zmqContainer.context = NULL;
		#if DEBUG
			printf("context freed\n");
		#endif
	}
	#if DEBUG
		printf("\nclosed\n");
	#endif
	return rc;
}
/** @} */ // end of zmqreqrepclient