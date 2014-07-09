/*
 * IoTKit client plugin to enable subscribe feature through Edison API
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
 * @file iotkit-client.h
 * @brief Headers of iotkit Async Client plugin for Edison API
 *
 * Provides features to connect to an MQTT Broker and subscribe to a topic
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "edisonapi.h"
#include "dlfcn.h"

#include <MQTTAsync.h>

#define CLIENTID    "IoTClient"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTAsync_token deliveredtoken;


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

char *interface = "edison-iotkit-client-interface"; // specifies the plugin interface json

void registerSensor(char *sensorname, char *type);
int init(void *serviceDesc);
int send(char *message, Context context);
int subscribe();
int unsubscribe(char *topic);
int receive(void (*) (char *topic, Context context));
int done();