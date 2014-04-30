/*
 * zmqreqrep-service.c
 *
 *  Created on: 2014-04-21
 *      Author: skothurx
 */

#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include "zmqreqrep-service.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 0
#endif

struct Holder {
	void *context;
	void *responder;
};

struct Holder zmqContainer;

int init(char *host, int port, char *type, void *sslargs) {
	zmqContainer.context = zmq_ctx_new();
	#if DEBUG
		printf("context initialised\n");
	#endif
    // This is server side
    char addr[128];
    sprintf(addr, "tcp://%s:%d", host, port);
    #if DEBUG
        printf("going to bind %s\n",addr);
    #endif
    zmqContainer.responder = zmq_socket(zmqContainer.context,ZMQ_REP);
	int rc = zmq_bind(zmqContainer.responder,addr);
    #if DEBUG
        printf("bind completed\n");
    #endif
	return rc;
}

int sendTo(void *client,char *message,Context context) {
	#if DEBUG
    	printf ("Sending message from Service %s...\n", message);
	#endif
	int rc = s_send (zmqContainer.responder, message);
	return rc;
}

int manageClient(void *client,Context context) {
    #if DEBUG
        printf ("In manageClient\n"");
    #endif
}

int receive(void (*handler)(void *client,char *message,Context context)) {
    #if DEBUG
        printf("In receive\n");
    #endif
    void *client;
    int rc = 0;
    //  Read message contents
    char *message = s_recv (zmqContainer.responder);
    #if DEBUG
        printf ("Received message in service: %s\n", message);
    #endif
    if (message == NULL) {
        rc = -1;
    }
    Context context;
    context.name = "event";
    context.value = "message";
    handler(client,message,context);
    free(message);
}

int publish(char *message,Context context) {
    #if DEBUG
        printf("In publish\n");
    #endif
}

int done() {
    int rc = -1;
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
