/*
 * zmq-reqrep.c
 *
 *  Created on: 2014-04-21
 *      Author: skothurx
 */

#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include <assert.h>
#include "zmq-reqrep.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 0
#endif

struct Holder {
	void *context;
	void *requester;
	void *responder;
	char *type;
};

struct Holder zmqContainer;

int createClient(char *host, int port, char *type, void *sslargs) {
	zmqContainer.context = zmq_ctx_new();
	zmqContainer.type = type;
	#if DEBUG
		printf("context initialised");
	#endif
	int rc = -1;
	if (strcmp(type,"req") == 0) {
		// This is client side
		zmqContainer.requester = zmq_socket(zmqContainer.context,ZMQ_REQ);
		char addr[128];
		sprintf(addr, "tcp://%s:%d", host, port);
		#if DEBUG
			printf("going to connect %s\n",addr);
		#endif
		rc = zmq_connect(zmqContainer.requester,addr);
		assert(rc == 0);
		#if DEBUG
			printf("connect completed");
		#endif
	} else {
		// This is server side
		char addr[128];
		sprintf(addr, "tcp://%s:%d", host, port);
		#if DEBUG
			printf("going to bind %s\n",addr);
		#endif
		zmqContainer.responder = zmq_socket(zmqContainer.context,ZMQ_REP);
		rc = zmq_bind(zmqContainer.responder,addr);
		assert (rc == 0);
		#if DEBUG
			printf("bind completed\n");
		#endif
	}
	#if DEBUG
		printf("\ncreateClient ended");
	#endif
	return rc;
}

int request(char *message) {
	#if DEBUG
    	printf ("Sending message %s...\n", message);
	#endif
	int rc = -1;
	if (strcmp(zmqContainer.type,"req") == 0) {
		rc = s_send (zmqContainer.requester, message);
	} else {
		rc = s_send (zmqContainer.responder, message);
	}
	return rc;
}

int response(void (*callback)(char *)) {
    //  Read message contents
    char *contents;
    int rc = 0;
    if (strcmp(zmqContainer.type,"req") == 0) {
        contents = s_recv (zmqContainer.requester);
    } else {
        contents = s_recv (zmqContainer.responder);
    }
    #if DEBUG
        printf ("Received message in server: %s\n", contents);
    #endif
    if (contents == NULL) {
        rc = -1;
    }
    callback(contents);
    free (contents);
    return rc;
}

int close() {
    int rc = -1;
	if (zmqContainer.requester != NULL) {
		rc = zmq_close(zmqContainer.requester);
		zmqContainer.requester = NULL;
		#if DEBUG
			printf("requester freed\n");
		#endif
	}
	if (zmqContainer.responder != NULL) {
		rc = zmq_close(zmqContainer.responder);
		zmqContainer.responder = NULL;
		#if DEBUG
			printf("responder freed\n");
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
