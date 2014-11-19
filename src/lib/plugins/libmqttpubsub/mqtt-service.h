/*
 * MQTT service plugin to enable publish feature through iotkit-comm API
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

/**
* @file mqtt-service.h
* @brief Headers of MQTT Async Service plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and publish a topic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <MQTTClient.h>

#include "iotkit-comm.h"

#define CLIENTID    "MQTTClient"
#define QOS         1
#define TIMEOUT     10000L

#ifndef DEBUG
    #define DEBUG 0
#endif

void *handle = NULL;
char *err = NULL;

char clientID[256];

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_SSLOptions sslopts = MQTTClient_SSLOptions_initializer;

char *interface = "service-interface"; // specifies the plugin interface json
bool provides_secure_comm = true;

int init(void *publishServiceDesc, Crypto *crypto);
int sendTo(void *client, char *message, Context context);
int publish(char *message,Context context);
int manageClient(void *client,Context context);
int receive(void (*publishServiceHandler)(void *client,char *message,Context context));
int done();
