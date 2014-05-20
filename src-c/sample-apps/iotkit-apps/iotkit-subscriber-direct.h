/*
 * edison-mqtt.h
 *
 *  Created on: 2014-04-16
 *      Author: pchenthx
 */

#ifndef EDISON_MQTT_ASYNC_H_
#define EDISON_MQTT_ASYNC_H_

#include "iotkit_pubsub.h"
#include <MQTTAsync.h>

#define CLIENTID    "ExampleClientP12"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTAsync_token deliveredtoken;


volatile int toStop = 0;
 volatile int finished = 0;
 volatile int connected = 0;
 volatile int quietMode = 0;
 volatile int sent = 0;
 volatile int delivery = 0;


#endif /* EDISON_MQTT_ASYNC_H_ */
