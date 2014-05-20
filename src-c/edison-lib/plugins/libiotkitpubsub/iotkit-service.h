/*
 * The IoT Kit Agent publishes data to Cloud through Edison API
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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "edisonapi.h"

#include <MQTTAsync.h>

char *interface = "edison-iotkit-service-interface";

#define CLIENTID    "ExampleIoTKITPub"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTAsync_token deliveredtoken;


#ifndef DEBUG
    #define DEBUG 1
#endif

 volatile int toStop = 0;
 volatile int finished = 0;
 volatile int connected = 0;
 volatile int quietMode = 0;
 volatile int sent = 0;
 volatile int delivery = 0;

int registerSensor(char *sensorname, char *type, char *unit);
int init(void *serviceDesc);
int sendTo(char *message,Context context);
int publish(char *topic, char *message);
int manageClient(char *topic);
int receive(void (*) (char *message, Context context));
int done();
