/*
 * edison_pubsub.h
 *
 *  Created on: 2014-04-17
 *      Author: pchenthx
 */

#ifndef EDISON_PUBSUB_H_
#define EDISON_PUBSUB_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int publish(char *topic, char *message);
int subscribe(char *topic, void (*callback)(char *topic, char *message));
int close();

//values for type --> open, ssl
int createClient(char *host, int port, char *type, void *sslargs);


#endif /* EDISON_PUBSUB_H_ */
