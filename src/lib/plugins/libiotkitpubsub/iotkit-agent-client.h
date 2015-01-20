/*
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
* @file iotkit-agent-client.h
* @brief Headers of iotkit Async Client plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and subscribe to a topic.
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
    #define DEBUG 1
#endif

volatile int toStop = 0;
volatile int finished = 0;
volatile int subscribed = 0;
volatile int connected = 0;
volatile int quietMode = 0;
volatile int sent = 0;
volatile int delivery = 0;

int clientInstanceNumber = 0;

char *interface = "client-interface"; // specifies the plugin interface json
bool provides_secure_comm = true;
bool communicates_via_proxy = true;

/**
 * Topic to publish or subscribe data to. This is the only topic
 * the cloud supports.
 */
char *subscribe_topic = "data";

void *handle = NULL;
char *err = NULL;

MQTTAsync client;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

void (*msgArrhandler) (char *topic, Context context) = NULL;

int init(void *serviceDesc, Crypto *crypto);
int send(char *message, Context context);
int receive(void (*) (char *topic, Context context));
int done();
