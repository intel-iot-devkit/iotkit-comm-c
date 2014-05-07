/*
 * MQTT Async service plugin to enable publish feature through Edison API
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

#ifndef EDISON_MQTT_ASYNC_H_
#define EDISON_MQTT_ASYNC_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "edisonapi.h"

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

char *interface = "edison-service-interface"; // specifies the plugin interface json

int init(void *serviceDesc);
int sendTo(void *client, char *message, Context context);
int publish(char *message, Context context);
int manageClient(void *client, Context context);
int receive(void (*) (void *client, char *message, Context context));
int done();


#endif /* EDISON_MQTT_ASYNC_H_ */
