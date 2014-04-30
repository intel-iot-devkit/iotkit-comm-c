/*
 * zmqpubsub-server.h
 *
 *  Created on: 2014-04-18
 *      Author: skothurx
 */

#include "../inc/common.h"

int init(char *host,int port,char *type,void *sslargs);
int sendTo(void *,char *,Context context);	// int sendTo(client, message, context) // for example, incase of mqtt... int sendTo(<<mqtt client>>, message, context);
int publish(char *,Context context); // int publish(message,context)
int manageClient(void *,Context context); // int manageClient(client,context) // for example, incase of mqtt... int manageClient(<<mqtt client>>, context);
int receive(void (*)(void *,char *,Context context)); // int receive(handler) // handler takes 3 parameters
int done();