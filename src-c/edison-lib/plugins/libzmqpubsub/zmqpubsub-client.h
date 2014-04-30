/*
 * zmqpubsub-client.h
 *
 *  Created on: 2014-04-18
 *      Author: skothurx
 */

#include "../inc/common.h"

int init(char *host,int port,char *type,void *sslargs);
int send(char *, Context);	// int send(message, context)
int subscribe(char *);	// int subscribe(topic)
int unsubscribe(char *);	// int unsubscribe(topic)
int receive(void (*)(char *, Context)); // int receive(handler) // handler takes 2 parameters
int done();
