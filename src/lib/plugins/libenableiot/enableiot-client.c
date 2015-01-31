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

#include "enableiot-client.h"

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
    #if DEBUG
        printf("Connect failed, code: %d\n", response->code);
        if(response->message) {
            printf("Connect failed, message: %s\n", response->message);
        }
    #endif
    finished = 1;
}

void onConnect(void* context, MQTTAsync_successData* response) {
    #if DEBUG
        printf("Connected\n");
    #endif

    connected = 1;

    #if DEBUG
        printf("Subscribing to topic: %s\n", subscribe_topic);
    #endif
    subscribe();
}

void connectionLost(void *context, char *cause) {
    MQTTAsync client2 = (MQTTAsync)context;
//    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;
    int rc;

    #if DEBUG
        printf("\nConnection lost\n");
        printf("     cause: %s\n", cause);
        printf("Reconnecting\n");
    #endif

    conn_opts.ssl = &sslopts;
    conn_opts.username = username;
    conn_opts.password = password;
    conn_opts.ssl->enableServerCertAuth = 0;

    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client2;
    conn_opts.keepAliveInterval = 300; // 5 minutes
    conn_opts.retryInterval = 0;
    if ((rc = MQTTAsync_connect(client2, &conn_opts)) != MQTTASYNC_SUCCESS) {
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

/*
 * @name Subscribes to a topic
 * @brief Subscribes to a topic with an MQTT broker.
 * @param[in] topic needs to be subscribed to
 * @return boolean, which specifies whether successfully subscribed or not
*/
int subscribe() {
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    int rc = 0;

    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;

    if ((rc = MQTTAsync_subscribe(client, strdup(subscribe_topic), QOS, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        exit(-1);
    }

    return rc;
}

/**
 * @name Cleanup the MQTT client
 * @brief Used to close the connections and for cleanup activities.
 * @return boolean, which specifies whether the connection is disconnected or not
 */
int done() {
    #if DEBUG
        printf("Invoked done()\n");
    #endif

    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

    int rc = 0;

    unsubscribe();

    opts.onSuccess = onDisconnect;
    opts.context = client;

    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(-1);
    }
    finished = 1;
    toStop = 0;

    MQTTAsync_destroy(&client);

    iotkit_cleanup();

    return rc;
}

/*
 * @name Unsubscribe a topic
 * @brief Discontinues the subscription to a topic.
 * @param[in] topic that has been previously subscribed to
*/
int unsubscribe() {

    #if DEBUG
        printf("Invoked MQTT: unsubscribe()\n");
    #endif

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    opts.onSuccess = onUnSubscribe;
    opts.onFailure = onUnSubscribeFailure;
    opts.context = client;

    int rc = 0;

    if ((rc = MQTTAsync_unsubscribe(client, subscribe_topic, &opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to unsubscribe, return code %d\n", rc);
        exit(-1);
    }

    return rc;
}

void initializeDeviceCredentials() {
    char *config_path = "/usr/share/iotkit-lib/device_config.json";
    cJSON *json = NULL, *jitem = NULL;
    FILE *fp = NULL;
    char *out;

    fp = fopen(config_path, "rb");

    if (fp == NULL) {
        #if DEBUG
            fprintf(stderr,"Error can't open file %s\n", config_path);
        #endif
    } else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
        } else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                fprintf(stderr,"%s\n", out);
                free(out);
                out = NULL;
            #endif

            if (!isJsonObject(json)) {
                fprintf(stderr,"invalid JSON format for %s file\n", config_path);
                goto endParseStateConfig;
            }

            jitem = cJSON_GetObjectItem(json, "deviceId");
            if (isJsonString(jitem)) {
                username = strdup(jitem->valuestring);
                #if DEBUG
                    printf("username = %s\n", username);
                #endif
            }

            jitem = cJSON_GetObjectItem(json, "deviceToken");
            if (isJsonString(jitem)) {
                password = strdup(jitem->valuestring);
                #if DEBUG
                    printf("password = %s\n", password);
                #endif
            }

            jitem = cJSON_GetObjectItem(json, "data_account_id");
            if (isJsonString(jitem)) {
                data_account_id = strdup(jitem->valuestring);
                #if DEBUG
                    printf("data_account_id = %s\n", data_account_id);
                #endif
            }


endParseStateConfig:
            cJSON_Delete(json);
        }

        free(buffer);
        fclose(fp);
    }
}

#if DEBUG
    void handleTrace(enum MQTTASYNC_TRACE_LEVELS level, char* message) {
        printf("%s\n", message);
    }
#endif

/**
 * @name Subscribe to a topic
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
int receive(void (*handler) (char *topic, Context context)) {
    int rc = 0;
    char uri[256];
    int i = 0;
    MQTTAsync_token token;
    MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;

    #if DEBUG
        printf("Invoked enableiot-client: receive()\n");
    #endif

    initializeDeviceCredentials();

    address = strdup("broker.us.enableiot.com");
    port = 8883;

    sprintf(uri, "ssl://%s:%d", address, port);
    #if DEBUG
        printf("MQTT URI is: %s\n", uri);
    #endif
    conn_opts.ssl = &sslopts;
    conn_opts.username = username;
    conn_opts.password = password;

    conn_opts.ssl->enableServerCertAuth = 0;

    char clientID[256];
    sprintf(clientID, "%s%d", CLIENTID, clientInstanceNumber++);

    MQTTAsync_create(&client, uri, clientID, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);

    #if DEBUG
        MQTTAsync_setTraceCallback(handleTrace);
        MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);
    #endif

    MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, deliveryComplete);

    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    conn_opts.keepAliveInterval = 300; // 5 minutes
    conn_opts.retryInterval = 0;

    strcpy(subscribe_topic, "server/metric/");
    if(data_account_id) {
        strcat(subscribe_topic, data_account_id);
        strcat(subscribe_topic, "/");
    }

    if(targetDeviceId) {
        strcat(subscribe_topic, targetDeviceId); // subscribe to prescribed device
    } else {
        strcat(subscribe_topic, username); // subscribe to own device
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

    msgArrhandler = handler;

    return 1;
}

long long getCurrentTimeInMillis() {
    long elapsedtime = -1L;
    long long currentTimeInMills;

    time(&elapsedtime);

    currentTimeInMills = (long long)elapsedtime * 1000L;

    return currentTimeInMills;
}

/**
 * @name retrieve a sensor data
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
char *retrieve(char *sensorName, char *deviceID, long long from, long long to) {
    char *response = NULL;
    RetrieveData *retrieveObj;
    char *sensorID = getSensorComponentId(strdup(sensorName));
    long long fromTimestamp = 0, toTimestamp = 0;

    #if DEBUG
        printf("Invoked enableiot-client: retrieve()\n");
    #endif

    if(from > 0) {
        fromTimestamp = from;
    }

    if(to <= 0) {
        toTimestamp = getCurrentTimeInMillis();
    } else {
        toTimestamp = to;
    }

    retrieveObj = createRetrieveDataObject(fromTimestamp, toTimestamp);

    if(deviceID) {
        addDeviceId(retrieveObj, strdup(deviceID));
    }

    if(sensorID) {
        addSensorId(retrieveObj, strdup(sensorID));
    }

    response = retrieveData(retrieveObj);

    return response;
}


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
    }
}

/**
 * @name Initialization
 * @brief Create and initialize the plugin.
 * @param[in] servQuery is the service query being queried for
 * @param[in] crypto authentication object
 * @return int, which specifies whether the connection is successfully established or not
 *
 * Establishes the connection with an MQTT broker.
 */
int init(void *servQuery, Crypto *crypto) {
    #if DEBUG
        printf("Invoked init()\n");
    #endif

    ServiceQuery *serviceQuery = (ServiceQuery *) servQuery;

    iotkit_init();

    address = serviceQuery->address;
    port = serviceQuery->port;
    parseServiceName(serviceQuery->service_name);

    targetDeviceId = serviceQuery->type_params.deviceid;

    return 0;
}

char *signIn(char *username, char *passwd) {
    char *response = NULL;
    response = getUserJwtToken(username, passwd);
    return response;
}
