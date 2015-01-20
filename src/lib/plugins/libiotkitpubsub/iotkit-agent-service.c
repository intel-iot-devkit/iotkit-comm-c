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
* @file iotkit-agent-service.c
* @brief Implementation of IoTKit Async Service plugin for iotkit-comm API.
*
* Provides features to connect to an MQTT Broker and publish to a topic.
*/

#include "iotkit-agent-service.h"

void handleSignal(int sig) {
    #if DEBUG
        printf("got interruption signal");
    #endif

    toStop = 1;
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

    registerSensor(sensorName, sensorType);
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
/**
 * @name Publish a message
 * @brief Used to send message to a broker.
 * @param[in] message to be published
 * @param[in] context w.r.t topic the message required to be published
 * @return boolean, specifies whether the message is successfully published or not
 */
int publish(char *message, Context context) {

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

/**
 * @name Cleanup the MQTT client
 * @brief Used to close the connections and for cleanup activities.
 * @return boolean, which specifies whether the connection is disconnected or not
 */
int done() {

    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

    int rc = 0;

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

/**
 * @name Subscribe to a topic
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
int receive(void (*handler) (char *topic, Context context)) {

    #if DEBUG
        printf("Invoked iotkit-service: setReceivedMessageHandler()\n");
    #endif

    return 1;
}

#if DEBUG
    void handleTrace(enum MQTTASYNC_TRACE_LEVELS level, char* message) {
        printf("%s\n", message);
    }
#endif

void parseServiceName(char *serviceName) {
    int tokenSize = 0;
    char *topic_name = serviceName;
    char *needle = NULL;

    if(strstr(topic_name, "/") == NULL) {
        fprintf(stderr, "%s has an incorrect format. Correct format is [namespace]/sensorType/sensorName", topic_name);
        exit(-1);
    }

    while((needle = strstr(topic_name, "/")) != NULL) {
        tokenSize ++;
        topic_name = needle + 1;
    }

    topic_name = needle = serviceName;
    while(tokenSize > 1) {
        needle = strstr(topic_name, "/") + 1;
        tokenSize --;
        topic_name = needle;
    }

    if(tokenSize == 1 && needle != NULL) {
        char *needle2 = strstr(topic_name, "/");
        int sensorTypeSize = needle2 - needle;
        sensorTypeSize += 1;
        sensorType = (char *)malloc(sizeof(char) * sensorTypeSize);
        strncpy(sensorType, topic_name, sensorTypeSize-1);
        sensorType[sensorTypeSize-1] = '\0';

        sensorName = (char *)malloc(sizeof(char) * strlen(needle2));
        needle2 += 1;
        strcpy(sensorName, needle2);

        printf("Sensor Name is %s\n", sensorName);
        printf("Sensor Type is %s\n", sensorType);
    }
}

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

    if(serviceQuery->port == 0)
        serviceQuery->port = 1884;

    if (serviceQuery->address != NULL) {
        sprintf(uri, "tcp://%s:%d", serviceQuery->address, serviceQuery->port);
    } else {
        sprintf(uri, "tcp://localhost:%d", serviceQuery->port);
    }

    // Default settings:
    int i = 0;

    MQTTAsync_token token;

    quietMode = 0;

    char clientID[256];
    sprintf(clientID, "%s%d", CLIENTID, clientInstanceNumber++);

    parseServiceName(serviceQuery->service_name);

    MQTTAsync_create(&client, uri, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    #if DEBUG
        MQTTAsync_setTraceCallback(handleTrace);
        MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);
    #endif

    MQTTAsync_setCallbacks(client, client, connectionLost, NULL, deliveryComplete);

    conn_opts.cleansession = 0;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.keepAliveInterval = 0;
    conn_opts.retryInterval = 0;

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

/**
 * @name Registers a sensor
 * @brief Registers a sensor with IoT Cloud through IoTKit Agent.
 * @param[in] sensorname is the name of the sensor on the device
 * @param[in] type denotes the datatype of the sensor values
 */
void registerSensor(char *sensorname, char *type) {
    char mesg[256];
    Context context;
    context.name = "topic";
    context.value = "data";

    sprintf(mesg, "{\"n\":\"%s\",\"t\":\"%s\"}", sensorname,type);

    #if DEBUG
        printf("Registering Sensor: %s\n", mesg);
    #endif

    publish(mesg, context);
}
