/*
 * zmq-pubsub.c
 *
 *  Created on: 2014-04-18
 *      Author: skothurx
 */

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "zmq-pubsub.h"
#include "../inc/zhelpers.h"

#ifndef DEBUG
#define DEBUG 0
#endif

struct Holder {
	void *context;
	void *publisher;
	void *subscriber;
};

struct Holder zmqContainer;

int createClient(char *host, int port, char *type, void *sslargs) {
	#if DEBUG
		printf("In createClient");
	#endif
	zmqContainer.context = zmq_ctx_new();
	#if DEBUG
		printf("context initialised");
	#endif
	int rc = -1;
	if (strcmp(type,"pub") == 0) {
		zmqContainer.publisher = zmq_socket(zmqContainer.context,ZMQ_PUB);
		char addr[128];
		sprintf(addr, "tcp://%s:%d", host, port);
		#if DEBUG
			printf("going to bind %s\n",addr);
		#endif
		rc = zmq_bind(zmqContainer.publisher,addr);
		#if DEBUG
			printf("bind completed");
		#endif
	} else {
		char addr[128];
		sprintf(addr, "tcp://%s:%d", host, port);
		#if DEBUG
			printf("going to connect %s\n",addr);
		#endif
		zmqContainer.subscriber = zmq_socket(zmqContainer.context,ZMQ_SUB);
		rc = zmq_connect(zmqContainer.subscriber,addr);
		#if DEBUG
			printf("subscriber connected\n");
		#endif
	}
	#if DEBUG
		printf("\ncreateClient ended");
	#endif
    return rc;
}

char *concat(char *topic,char *mesg)
{
	#if DEBUG
		printf("topic: %s\n",topic);
		printf("mesg: %s\n",mesg);
	#endif
    char *buffer = malloc((strlen(topic) + strlen(mesg) + 2) * sizeof(char)); // additional 2 bytes
    												//for empty space & null termination character
    char *result = buffer;
    while(*topic) {
    	*buffer++ = *topic++;
    }
    *buffer++ = ' '; // Inserting an Empty space char to be used as a Delimiter between Topic & Message
    while(*mesg)
    {
        *buffer++ = *mesg++;
    }
    *buffer = '\0';
	#if DEBUG
    	printf("Concatenated String: %s\n",result);
	#endif
    return result;
}

int publish(char *topic, char *message) {
	#if DEBUG
		printf("publish started");
	#endif
	char *string = concat(topic,message);
	int rc = s_send (zmqContainer.publisher, string);
	#if DEBUG
		printf("Publisher Finished\n");
	#endif
	return rc;
}

int subscribe(char *topic, void (*callback)(char *, char *)) {
	#if DEBUG
		printf("\nsubscribing for topic: %s\n",topic);
	#endif
	//  Subscribe only for the topic
	int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_SUBSCRIBE, topic, strlen(topic));
	//  Read message contents
	char *contents = s_recv (zmqContainer.subscriber);
	#if DEBUG
		printf ("Received: %s\n", contents);
	#endif
	char *message = strstr(contents," "); // excluding the 'topic'
	message++;
	callback(topic,message);
	free (contents);
    return rc;
}

int unsubscribe(char *topic) {
	#if DEBUG
		printf("\nunsubscribing the topic: %s\n",topic);
	#endif
    int rc = zmq_setsockopt (zmqContainer.subscriber, ZMQ_UNSUBSCRIBE, topic, strlen(topic));
    return rc;
}

int createService() {
    #if DEBUG
        printf("In createService\n");
    #endif
}

int setReceivedMessageHandler() {
    #if DEBUG
        printf("In setReceivedMessageHandler\n");
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
