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

#include "enableiot-service.h"


/**
 * @name Publish a message
 * @brief Used to send message to a broker.
 * @param[in] message to be published
 * @param[in] context w.r.t topic the message required to be published
 * @return boolean, specifies whether the message is successfully published or not
 */
int publish(char *message, Context context) {
    cJSON *json = NULL, *jitem = NULL;
    char *name;
    char valuestr[256];
    char *out;
    int value;

    name = NULL;

    json = cJSON_Parse(message);

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
            fprintf(stderr,"Invalid JSON format\n");
            goto endParseStateConfig;
        }

        jitem = cJSON_GetObjectItem(json, "name");
        if (!isJsonString(jitem)) {
            fprintf(stderr,"Invalid JSON format. Missing \"name\"\n");
            goto endParseStateConfig;
        }

        name = strdup(jitem->valuestring);
        #if DEBUG
            printf("component name = %s\n", name);
        #endif

        jitem = cJSON_GetObjectItem(json, "value");
        if (!isJsonNumber(jitem)) {
            fprintf(stderr,"Invalid JSON format. Missing \"value\"\n");
            goto endParseStateConfig;
        }

        value = jitem->valueint;
        sprintf(valuestr, "%d", value);

        #if DEBUG
            printf("component value = %d\n", value);
        #endif

        submitData(name, valuestr);

endParseStateConfig:
        cJSON_Delete(json);
    }

    return 0;
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

    return 0;
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

char *activateDevice() {
    bool isActivated = false;

    iotkit_init();

    isActivated = isDeviceActivated(); // if device is already active; no need to activate again

    if(!isActivated) {
        if(!activationCode) {
            fprintf(stderr, "activation Code cannot be NULL\n");
            return NULL;
        }

        if(!deviceID) {
            fprintf(stderr, "device ID cannot be NULL\n");
            return NULL;
        }

        char *response = NULL;
        response = activateADevice2(activationCode, deviceID);

        iotkit_cleanup();
        return response;
    }

    iotkit_cleanup();
    return NULL;
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
    ServiceQuery *serviceQuery = (ServiceQuery *) servQuery;
    #if DEBUG
        printf("Invoked init()\n");
    #endif

    activationCode = serviceQuery->type_params.activationCode;
    deviceID = serviceQuery->type_params.deviceid;

    #if DEBUG
        printf("Activation Code is : %s\n", activationCode);
        printf("Device ID is : %s\n", deviceID);
    #endif

    parseServiceName(serviceQuery->service_name);

    activateDevice();

    registerSensor();

    return 0;
}

/**
 * @name Registers a sensor
 * @brief Registers a sensor with IoT Cloud
 */
char *registerSensor() {
    char *response = NULL;

    iotkit_init();

    response = addComponent(sensorName, sensorType);

    iotkit_cleanup();
    return response;
}
