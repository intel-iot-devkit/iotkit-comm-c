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

#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include "zmqreqrep-client.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 0
#endif

struct Holder {
	void *context;
	void *requester;
};

struct Holder zmqContainer;

int init(void *servQuery) {
	#if DEBUG
		printf("context initialised\n");
	#endif
	ServiceQuery *serviceQuery = (ServiceQuery *)servQuery;
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

int send(char *message,Context context) {
	#if DEBUG
    	printf ("Sending message from Client %s...\n",message);
	#endif
	int rc = s_send(zmqContainer.requester, message);
	return rc;
}

int subscribe(char *topic) {
	#if DEBUG
		printf("In subscribe\n");
	#endif
}

int unsubscribe(char *topic) {
	#if DEBUG
		printf("In unsubscribe\n");
	#endif
}

int receive(void (*handler)(char *message, Context)) {
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
    handler(contents,context);
    free(contents);
}

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
