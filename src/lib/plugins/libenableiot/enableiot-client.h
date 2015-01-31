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
* @file enableiot-client.h
* @brief Headers of enableiot plugin for iotkit-comm API.
*
* Provides features to connect to enableiot cloud and subscribe to a device
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <MQTTAsync.h>
#include <cJSON.h>

#include "iotkit-comm.h"
#include "util.h"

#include "authorization.h"
#include "data_api.h"

#define CLIENTID    "IoTClient"
#define QOS         0
#define TIMEOUT     10000L

#ifndef DEBUG
    #define DEBUG 0
#endif

volatile int toStop = 0;
volatile int finished = 0;
volatile int subscribed = 0;
volatile int connected = 0;
volatile int sent = 0;

int clientInstanceNumber = 0;

char *interface = "enableiot-client-interface"; // specifies the plugin interface json
bool provides_secure_comm = true;
bool communicates_via_proxy = true;

char *address = NULL;
int port = 0;
char *username = NULL; // aka device ID
char *password = NULL; // aka device Token
char *data_account_id = NULL; // data account ID
char *sensorName = NULL;
char *sensorType = NULL;
char *targetDeviceId = NULL;

/**
 * Topic to ubscribe data
 */
char subscribe_topic[256];

void *handle = NULL;
char *err = NULL;

MQTTAsync client;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

void (*msgArrhandler) (char *topic, Context context) = NULL;

int init(void *serviceDesc, Crypto *crypto);
int subscribe();
int publish(char *message,Context context);
int receive(void (*) (char *topic, Context context));
//char *signIn(char *username, char *passwd);
//char *retrieve(char *sensorName, char *deviceID, long long from, long long to);
int done();
