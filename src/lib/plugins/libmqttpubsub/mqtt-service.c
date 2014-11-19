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
* @file mqtt-service.c
* @brief Headers of MQTT Async Service plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and publish a topic.
*/

#include "mqtt-service.h"

void connectionLost(void *context, char *cause) {
    ServiceQuery *serviceQuery = (ServiceQuery *)context;
    int rc;

    printf("Connection lost due to :%s\n", cause);
    printf("Reconnecting...\n");

    conn_opts.cleansession = 1;
    conn_opts.keepAliveInterval = 20;
    conn_opts.retryInterval = 1000;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        exit(1);
    }
}

int init(void *publishServiceDesc, Crypto *crypto) {
    ServiceSpec *serviceSpec = (ServiceSpec *) publishServiceDesc;
    int rc = 0;
    char uri[256];

    if (isPresentPropertyInCommParams(serviceSpec, "ssl") == true && \
        strcasecmp(getValueInCommParams(serviceSpec, "ssl"), "true") == 0) {
        sprintf(uri, "ssl://%s:%d", serviceSpec->address, serviceSpec->port);

        conn_opts.ssl = &sslopts;

        if (isPresentPropertyInCommParams(serviceSpec, "keyStore")) {
            conn_opts.ssl->keyStore = getValueInCommParams(serviceSpec, "keyStore");
        }
        if (isPresentPropertyInCommParams(serviceSpec, "privateKey")) {
            conn_opts.ssl->privateKey = getValueInCommParams(serviceSpec, "privateKey");
        }
        if (isPresentPropertyInCommParams(serviceSpec, "trustStore")) {
            conn_opts.ssl->trustStore = getValueInCommParams(serviceSpec, "trustStore");
        }

        conn_opts.ssl->enableServerCertAuth = 0;
    } else if(serviceSpec->type_params.mustsecure) {
        if(crypto->host) {
            sprintf(uri, "ssl://%s:%d", crypto->host, crypto->mosquittoSecurePort);
            conn_opts.ssl = &sslopts;
            conn_opts.ssl->trustStore = strdup(crypto->cacert);
            conn_opts.ssl->privateKey = strdup(crypto->userkey);
            conn_opts.ssl->keyStore = strdup(crypto->usersslcert);

            conn_opts.ssl->enableServerCertAuth = 0;
        } else {
            printf("Cannot secure communication channel."
                         " Please setup and configure credentials using iotkit-comm setupAuthentication.");
            return -1;
        }
    } else {
        if (serviceSpec->address != NULL) {
            sprintf(uri, "tcp://%s:%d", serviceSpec->address, serviceSpec->port);
        } else {
            sprintf(uri, "tcp://localhost:%d", serviceSpec->port);
        }
    }

    // Default settings:
    sprintf(clientID, "%s_%d", CLIENTID, getpid());

    MQTTClient_create(&client, uri, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, serviceSpec, connectionLost, NULL, NULL);

    conn_opts.cleansession = 0;
    conn_opts.keepAliveInterval = 20;
    conn_opts.retryInterval = 0;

    if (isPresentPropertyInCommParams(serviceSpec, "username") == true && \
        isPresentPropertyInCommParams(serviceSpec, "password") == true) {
        conn_opts.username = getValueInCommParams(serviceSpec, "username");
        conn_opts.password = getValueInCommParams(serviceSpec, "password");
    }

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        exit(1);
    }

    return rc;
}

int sendTo(void *client, char *message, Context context) {
    #if DEBUG
        printf("In sendTo\n");
    #endif
    return -1;
}

/**  Publishing a message. This function will publish message to the clients.
* @param message a string message
* @param context a context object
* @return The result code
*/
int publish(char *message,Context context) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int rc = 0;
    char *topic;

    if (context.name != NULL && context.value != NULL && strcmp(context.name, "topic") == 0) {
        topic = context.value;
    } else {
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

int manageClient(void *client,Context context) {
    #if DEBUG
        printf("In manageClient\n");
    #endif
    return -1;
}

int receive(void (*publishServiceHandler) (void *client,char *message,Context context)) {
    #if DEBUG
        printf("In receive\n");
    #endif
    return -1;
}

/** Cleanup function. This function close the publisher socket and destroy the context object.
* @return The result code
*/
int done() {
    int rc = 0;

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(-1);
    }

    MQTTClient_destroy(&client);

    return rc;
}
