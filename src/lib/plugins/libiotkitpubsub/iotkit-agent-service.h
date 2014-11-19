/*
 * IoTKit Async service plugin to enable publish feature through iotkit-comm API
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
* @file iotkit-agent-service.h
* @brief Headers of iotkit Async Service plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and publish a topic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <MQTTAsync.h>

#include "iotkit-comm.h"

#define CLIENTID    "IoTClient"
#define QOS         1
#define TIMEOUT     10000L

#ifndef DEBUG
    #define DEBUG 0
#endif

volatile int toStop = 0;
volatile int finished = 0;
volatile int subscribed = 0;
volatile int connected = 0;
volatile int quietMode = 0;
volatile int sent = 0;
volatile int delivery = 0;

int clientInstanceNumber = 0;

char *interface = "iotkit-agent-service-interface"; // specifies the plugin interface json
bool provides_secure_comm = true;

void *handle = NULL;
char *err = NULL;

MQTTAsync client;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

void registerSensor(char *sensorname, char *type);
int init(void *serviceDesc, Crypto *crypto);
int sendTo(void *client, char *message, Context context);
int publish(char *message,Context context);
int manageClient(void *client,Context context);
int receive(void (*) (char *topic, Context context));
int done();
