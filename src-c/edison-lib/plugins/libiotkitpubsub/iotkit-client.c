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
 * @file iotkit-client.c
 * @brief Implementation of IoTKit Client plugin for Edison API
 *
 * Provides features to connect to an MQTT Broker and subscribe to a topic
 */

#include "iotkit-client.h"

void *handle=NULL;
char *err=NULL;

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;


void (*msgArrhandler) (char *topic, Context context) = NULL;

 int messageArrived(void *ctx, char *topicName, int topicLen, MQTTClient_message *message)
 {
    char *payloadmsg;
    Context context;

    #if DEBUG
        char* payloadptr;
        int i;
        printf("Message arrived\n");
        printf("topic: %s\n", topicName);
        printf("message:");

        payloadptr = message->payload;
        for(i=0; i<message->payloadlen; i++)
        {
            putchar(*payloadptr++);
        }
        putchar('\n');
    #endif

    payloadmsg = (char *)malloc(message->payloadlen+1);
    strncpy(payloadmsg, message->payload, message->payloadlen);
    payloadmsg[message->payloadlen] = '\0';


    context.name = "topic";
    context.value = strdup(topicName);
    if(msgArrhandler != NULL){
        msgArrhandler(payloadmsg, context);
    } else {
        printf("error: Receive Handler not set\n");
    }

    //MQTTClient_freeMessage(&message);
    free(payloadmsg);

    return true;
 }

 void connectionLost(void *context, char *cause)
 {
    MQTTClient client = (MQTTClient)context;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    printf("Connection lost due to :%s\n", cause);
    printf("Reconnecting...\n");

    conn_opts.cleansession = 1;
    conn_opts.keepAliveInterval = 20;
    conn_opts.retryInterval = 1000;
    //conn_opts.maxInflight= 30;

    /*
    // TODO: SSL based client needs to be implemented
    conn_opts.ssl = &sslopts;
    conn_opts.ssl->trustStore = "./certs/client.crt";
    conn_opts.ssl->keyStore = "./certs/client.key";
    conn_opts.ssl->enableServerCertAuth = 0;
    */

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        exit(1);
    }
 }

/**
 * @name publish a message
 * @brief used to send message to a broker
 * @param[in] message to be published
 * @param[in] context w.r.t topic the message required to be published
 * @return boolean specifies whether the message is successfully published or not
 */
 int send(char *message, Context context) {

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int rc = 0;
    char *topic;

    if(context.name != NULL && context.value != NULL && strcmp(context.name, "topic") == 0){
        topic = context.value;
    }
    else {
        printf("Topic not available in the send command");
        return MQTTCLIENT_NULL_PARAMETER;
    }

    pubmsg.payload = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);

    #if DEBUG
        printf("Waiting for up to %d seconds for publication of %s\n"
        "on topic %s for client with ClientID: %s\n",
        (int)(TIMEOUT/1000), message, topic, clientID);
    #endif

    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    #if DEBUG
    printf("Message with delivery token %d delivered\n", token);
    #endif

    return rc;
 }

/**
 * @name subscribes to a topic
 * @brief subscribes to a topic with an MQTT broker
 * @param[in] topic which needs to be subscribed to
 * @return boolean which specifies whether successfully subscribed or not
 */
 int subscribe() {
int rc = 0;
    char *topic = "data";

    if ((rc = MQTTClient_subscribe(client, topic, QOS)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        exit(-1);
    }

 	return rc;
 }

/**
 * @name cleanup the MQTT client
 * @brief used to close the connections and for cleanup activities
 * @return boolean which specifies whether the connection is disconnected or not
 */
 int done() {
    int rc = 0;

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(-1);
    }

    MQTTClient_destroy(&client);

    return rc;
 }

/**
 * @name unsubscribe a topic
 * @brief discontinues the subscription to a topic
 * @param[in] topic which has been previously subscribed to
 */
int unsubscribe(char *topic){

    #if DEBUG
        printf("Invoked MQTT: unsubscribe()\n");
    #endif

    int rc = 0;

    if ((rc = MQTTClient_unsubscribe(client, topic)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        exit(-1);
    }

    return rc;
}

/**
 * @name subscribe to a topic
 * @brief registers the client's callback to be invoked on receiving a message from MQTT broker
 * @param handler to be registered as a callback
 */
int receive(void (*handler) (char *topic, Context context)){

    #if DEBUG
        printf("Invoked MQTT: setReceivedMessageHandler()\n");
    #endif

    msgArrhandler = handler;

    return 1;
}

int clientInstanceNumber = 0;

/**
 * @name initialise the MQTT client
 * @brief initialises the plugin.
 * @param[in] serviceDesc is the service description being queried for
 * @return boolean which specifies whether the connection is successfully established or not
 *
 * Establishes the connection with an MQTT broker
 */
int init(void *serviceDesc)
{
    ServiceQuery *serviceQuery = (ServiceQuery *)serviceDesc;
    int rc = 0;
    char uri[256];

//        if(strcmp(type, "ssl") == 0){
//            sprintf(uri, "ssl://%s:%d", host, port);
//        }else {
        if(serviceQuery->address != NULL){
        sprintf(uri, "tcp://%s:%d", serviceQuery->address, serviceQuery->port);
        } else {
            sprintf(uri, "tcp://localhost:%d", serviceQuery->port);
        }
//      }
        // Default settings:
        char clientID[256];
        sprintf(clientID, "%s%d", CLIENTID, clientInstanceNumber++);

        MQTTClient_create(&client, uri, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        MQTTClient_setCallbacks(client, client, connectionLost, messageArrived, NULL);

        conn_opts.cleansession = 0;
        conn_opts.keepAliveInterval = 20;
        conn_opts.retryInterval = 0;
        //conn_opts.maxInflight= 30;

        /*
        // TODO: SSL based client needs to be implemented
        conn_opts.ssl = &sslopts;
        conn_opts.ssl->trustStore = "./certs/client.crt";
        conn_opts.ssl->keyStore = "./certs/client.key";
        conn_opts.ssl->enableServerCertAuth = 0;
        */

        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
        {
            printf("Failed to start connect, return code %d\n", rc);
            exit(1);
        }

    return rc;
}

/**
 * @name registers a sensor
 * @brief registers a sensor with IoT Cloud through IoTKit Agent
 * @param[in] sensorname is the name of the sensor on the device
 * @param[in] type denotes the datatype of the sensor values
 */
void registerSensor(char *sensorname, char *type) {
    char mesg[256];
    Context context;
    context.name = "topic";
    context.value = "data";

    sprintf(mesg, "{\"n\":%s,\"t\":%s}", sensorname,type);
    send(mesg, context);
}