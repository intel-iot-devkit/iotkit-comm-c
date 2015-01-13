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

/**
 * @name Create and initialize the MQTT client
 * @brief initializes the plugin.
 * @param[in] publishServiceDesc is the service query being queried for
 * @param[in] crypto authentication object
 *
 * Establishes the connection with an MQTT broker.
 */
int init(void *publishServiceDesc, Crypto *crypto) {
    ServiceSpec *serviceSpec = (ServiceSpec *) publishServiceDesc;
    int rc = 0;
    char uri[256];

    if(serviceSpec->port == 0)
        serviceSpec->port = 1883;

    if(serviceSpec->type_params.mustsecure) {
        if(crypto && crypto->host) {
            sprintf(uri, "ssl://%s:%d", crypto->host, crypto->mosquittoSecurePort);
            conn_opts.ssl = &sslopts;
            conn_opts.ssl->trustStore = strdup(crypto->cacert);
            conn_opts.ssl->privateKey = strdup(crypto->userkey);
            conn_opts.ssl->keyStore = strdup(crypto->usersslcert);

            conn_opts.ssl->enableServerCertAuth = 0;
        } else {
            printf("Cannot secure communication channel."
                         " Please setup and configure credentials using iotkit-comm setupAuthentication.\n");
            exit(1);
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

    default_topic = serviceSpec->service_name;

    return rc;
}

/**
 * @name send a message
 * @brief Used to send message to a client
 * @param[in] client to be communicated
 * @param[in] message to be published
 * @param[in] context w.r.t topic the message required to be published
 */
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
        topic = default_topic;
    }

    pubmsg.payload = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    #if DEBUG
        printf("Message \'%s\' published on topic \'%s\'\n", message, topic);
    #endif

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
