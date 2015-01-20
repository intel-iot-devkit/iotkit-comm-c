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
* @file iotkit-agent-client.c
* @brief Implementation of IoTKit Async Client plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and subscribe to a topic.
*/

#include "iotkit-agent-client.h"

void handleSignal(int sig) {
    #if DEBUG
        printf("got interruption signal");
    #endif

    toStop = 1;
}

int messageArrived(void *ctx, char *topicName, int topicLen, MQTTAsync_message *message) {
    int i;
    char* payloadptr;
    char *payloadmsg;
    Context context;

    #if DEBUG
        printf("Message arrived\n");
        printf("topic: %s\n", topicName);
        printf("message:");

        payloadptr = message->payload;
        for(i = 0; i<message->payloadlen; i++) {
            putchar(*payloadptr++);
        }
        putchar('\n');
    #endif

    payloadmsg = (char *)malloc(message->payloadlen+1);
    if (payloadmsg != NULL) {
        strncpy(payloadmsg, message->payload, message->payloadlen);
        payloadmsg[message->payloadlen] = '\0';
    }

    context.name = "topic";
    context.value = strdup(topicName);
    if (msgArrhandler != NULL) {
        msgArrhandler(payloadmsg, context);
    } else {
        printf("error: Receive Handler not set\n");
    }

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    if (payloadmsg != NULL) {
        free(payloadmsg);
        payloadmsg = NULL;
    }
    if (context.value != NULL) {
        free(context.value);
        context.value = NULL;
    }
    return 1;
}

void onSubscribe(void* context, MQTTAsync_successData* response) {
    #if DEBUG
        printf("Subscribe succeeded\n");
    #endif
    subscribed = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response) {
    #if DEBUG
        printf("Subscribe failed\n");
    #endif

    finished = 1;
}

void onUnSubscribe(void* context, MQTTAsync_successData* response) {
    #if DEBUG
        printf("UnSubscribe succeeded\n");
    #endif
    subscribed = 0;
}

void onUnSubscribeFailure(void* context, MQTTAsync_failureData* response) {
    #if DEBUG
        printf("UnSubscribe failed\n");
    #endif
}

void onDisconnect(void* context, MQTTAsync_successData* response) {
    #if DEBUG
        printf("Successful disconnection\n");
    #endif

    finished = 1;
}

void onSendFailure(void* context, MQTTAsync_failureData* response) {
    #if DEBUG
        printf("onSendFailure: message with token value %d delivery failed\n", response->token);
    #endif
}

void onSend(void* context, MQTTAsync_successData* response) {
    static last_send = 0;

    if (response->token - last_send != 1) {
        printf("Error in onSend, token value %d, last_send %d\n", response->token, last_send);
    }

    last_send++;

    if ((response->token % 1000) == 0) {
        printf("onSend: message with token value %d delivery confirmed\n", response->token);
    }
}

void deliveryComplete(void* context, MQTTAsync_token token) {
    sent++;
    if ((sent % 1000) == 0) {
        printf("deliveryComplete: message with token value %d delivery confirmed\n", token);
    }
    if (sent != token) {
        printf("Error, sent %d != token %d\n", sent, token);
    }
}

void onConnectFailure(void* context, MQTTAsync_failureData* response) {
    printf("Connect failed\n");
    finished = 1;
}

void onConnect(void* context, MQTTAsync_successData* response) {
    #if DEBUG
        printf("Connected\n");
    #endif

    connected = 1;

    subscribe(subscribe_topic);
}

void connectionLost(void *context, char *cause) {
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    #if DEBUG
        printf("\nConnection lost\n");
        printf("     cause: %s\n", cause);
        printf("Reconnecting\n");
    #endif

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.retryInterval = 1000;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
}

/**
 * @name Publish a message
 * @brief Used to send message to a broker.
 * @param[in] message to be published
 * @param[in] context w.r.t topic the message required to be published
 * @return boolean, specifies whether the message is successfully published or not
 */
int send(char *message, Context context) {

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

    int rc = 0;
    char *topic;

    if (context.name != NULL && context.value != NULL && strcmp(context.name, "topic") == 0) {
        topic = context.value;
    }
    else {
        printf("Topic not available in the send command\n");
        return MQTTASYNC_NULL_PARAMETER;
    }

    unsigned long i;
    struct timeval tv;
    gettimeofday(&tv,NULL);

    #if DEBUG
        printf("start seconds : %ld\n",tv.tv_sec);
    #endif

    opts.onSuccess = onSend;
    opts.onFailure = onSendFailure;
    opts.context = client;
    pubmsg.payload = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    usleep(100);

    if ((rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts))     \
            != MQTTASYNC_SUCCESS) {
        printf("Failed to start sendMessage, return code %d\n", rc);
        exit(-1);
    }

    gettimeofday(&tv,NULL);

    #if DEBUG
        printf("end seconds : %ld\n",tv.tv_sec);
    #endif

    return rc;
}

/*
 * @name Subscribes to a topic
 * @brief Subscribes to a topic with an MQTT broker.
 * @param[in] topic needs to be subscribed to
 * @return boolean, which specifies whether successfully subscribed or not
*/
int subscribe(char *topic) {
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    int rc = 0;

    if(!topic) {
        topic = "data";
    }

    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;

    if ((rc = MQTTAsync_subscribe(client, topic, QOS, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        exit(-1);
    }

    while (!subscribed) {
        sleep(1); // waiting for subscribe
    }

    return rc;
}

/**
 * @name Cleanup the MQTT client
 * @brief Used to close the connections and for cleanup activities.
 * @return boolean, which specifies whether the connection is disconnected or not
 */
int done() {

    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

    int rc = 0;

    unsubscribe(subscribe_topic);

    opts.onSuccess = onDisconnect;
    opts.context = client;

    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(-1);
    }
    finished = 1;
    toStop = 0;

    MQTTAsync_destroy(&client);

    return rc;
}

/*
 * @name Unsubscribe a topic
 * @brief Discontinues the subscription to a topic.
 * @param[in] topic that has been previously subscribed to
*/
int unsubscribe(char *topic) {

    #if DEBUG
        printf("Invoked MQTT: unsubscribe()\n");
    #endif

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    if(!topic) {
        topic = "data";
    }

    opts.onSuccess = onUnSubscribe;
    opts.onFailure = onUnSubscribeFailure;
    opts.context = client;

    int rc = 0;

    if ((rc = MQTTAsync_unsubscribe(client, topic, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        exit(-1);
    }

    while (subscribed) {
        sleep(1); // waiting for subscribe
    }

    return rc;
}

/**
 * @name Subscribe to a topic
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
int receive(void (*handler) (char *topic, Context context)) {

    #if DEBUG
        printf("Invoked MQTT: setReceivedMessageHandler()\n");
    #endif

    msgArrhandler = handler;

    return 1;
}

#if DEBUG
    void handleTrace(enum MQTTASYNC_TRACE_LEVELS level, char* message) {
        printf("%s\n", message);
    }
#endif

/**
 * @name Create the MQTT client
 * @brief Create and initialize the mqtt plugin.
 * @param[in] servQuery is the service query being queried for
 * @param[in] crypto authentication object
 * @return boolean, which specifies whether the connection is successfully established or not
 *
 * Establishes the connection with an MQTT broker.
 */
int init(void *servQuery, Crypto *crypto) {
    ServiceQuery *serviceQuery = (ServiceQuery *) servQuery;
    MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;
    int rc = 0;
    char uri[256];

    strcpy(uri, "tcp://localhost:1884");

    // Default settings:
    int i = 0;

    MQTTAsync_token token;

    quietMode = 0;

    char clientID[256];
    sprintf(clientID, "%s%d", CLIENTID, clientInstanceNumber++);

    MQTTAsync_create(&client, uri, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    #if DEBUG
        MQTTAsync_setTraceCallback(handleTrace);
        MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);
    #endif

    MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, deliveryComplete);

    conn_opts.cleansession = 0;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.keepAliveInterval = 0;
    conn_opts.retryInterval = 0;

    if(serviceQuery->type_params.deviceid) {
        subscribe_topic = serviceQuery->type_params.deviceid; // subscribe to prescribed device
    } else {
        subscribe_topic = strdup("data"); // subscribe to own device
    }

    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start connect, return code %d\n", rc);
        exit(1);
    }

    #if DEBUG
        printf("Waiting for connect\n");
    #endif

    while (connected == 0 && finished == 0 && toStop == 0) {
        #if DEBUG
            printf("Waiting for connect: %d %d %d\n", connected, finished, toStop);
        #endif

        sleep(1);
    }

    return rc;
}
