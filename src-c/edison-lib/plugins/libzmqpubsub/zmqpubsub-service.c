/*
 * ZMQ PUB/SUB plugin to enable publish feature through Edison API
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

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "zmqpubsub-service.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 1
#endif

struct Holder {
	void *context;
	void *publisher;
};

struct Holder zmqContainer;

int init(void *serviceDesc) {
	#if DEBUG
		printf("In createClient\n");
	#endif
	ServiceDescription *serviceDescription = (ServiceDescription *)serviceDesc;
	zmqContainer.context = zmq_ctx_new();
	#if DEBUG
		printf("context initialised\n");
	#endif
    zmqContainer.publisher = zmq_socket(zmqContainer.context,ZMQ_PUB);
    char addr[128];
    sprintf(addr, "tcp://%s:%d\n", serviceDescription->address, serviceDescription->port);
    #if DEBUG
        printf("going to bind %s\n",addr);
    #endif
    int rc = zmq_bind(zmqContainer.publisher,addr);
    #if DEBUG
        printf("bind completed\n");
    #endif
    return rc;
}

int sendTo(void *client, char *message, Context context) {
	#if DEBUG
		printf("In sendTo\n");
	#endif
}

int publish(char *message,Context context) {
	#if DEBUG
		printf("publish started\n");
	#endif
	#if DEBUG
   		printf("Publishing message %s\n",message);
   	#endif
	int rc = s_send(zmqContainer.publisher, message);
	#if DEBUG
		printf("Publisher Finished\n");
	#endif
	return rc;
}

int manageClient(void *client,Context context) {
    #if DEBUG
        printf("In manageClient\n");
    #endif
}

int receive(void (*handler)(void *,char *,Context)) {
    #if DEBUG
        printf("In receive\n");
    #endif
}

int done() {
	if (zmqContainer.publisher != NULL) {
		zmq_close(zmqContainer.publisher);
		zmqContainer.publisher = NULL;
		#if DEBUG
			printf("publisher freed\n");
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
