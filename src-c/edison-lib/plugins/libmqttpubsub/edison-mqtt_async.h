/*
 * edison-mqtt.h
 *
 *  Created on: 2014-04-16
 *      Author: pchenthx
 */

#ifndef EDISON_MQTT_ASYNC_H_
#define EDISON_MQTT_ASYNC_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <MQTTAsync.h>

#define CLIENTID    "ExampleClientPub"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTAsync_token deliveredtoken;


#ifndef DEBUG
    #define DEBUG 0
#endif

volatile int toStop = 0;
 volatile int finished = 0;
 volatile int connected = 0;
 volatile int quietMode = 0;
 volatile int sent = 0;
 volatile int delivery = 0;



int send(char *topic, char *message);
int subscribe(char *topic, void (*callback)(char *topic, char *message));
int unsubscribe(char *topic);
int setReceivedMessageHandler();
//values for type --> open, ssl
int createClient(char *host, int port, char *type, void *sslargs);
int createService();
int done();


#endif /* EDISON_MQTT_ASYNC_H_ */
