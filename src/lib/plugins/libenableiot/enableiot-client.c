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

/**
 * @name Cleanup the MQTT client
 * @brief Used to close the connections and for cleanup activities.
 * @return boolean, which specifies whether the connection is disconnected or not
 */
int done() {
    #if DEBUG
        printf("Invoked done()\n");
    #endif

    iotkit_cleanup();

    return 0;
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

long getCurrentTimeInSeconds() {
    long elapsedtime = -1L;

    time(&elapsedtime);

    return elapsedtime;
}

/**
 * @name retrieve a sensor data
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
char *retrieve(char *sensorName, char *deviceID, long from, long to) {
    char *response = NULL;
    RetrieveData *retrieveObj;
    char *sensorID = getSensorComponentId(strdup(sensorName));
    long fromTimestamp = 0, toTimestamp = 0;

    #if DEBUG
        printf("Invoked enableiot-client: retrieve()\n");
    #endif

    if(from > 0) {
        fromTimestamp = from;
    }

    if(to <= 0) {
        toTimestamp = getCurrentTimeInSeconds();
    } else {
        toTimestamp = to;
    }

    retrieveObj = createRetrieveDataObject(fromTimestamp, toTimestamp);

    #if DEBUG
        printf("Retrieve data between %lld till %lld\n", retrieveObj->fromMillis, retrieveObj->toMillis);
    #endif

    if(deviceID) {
        addDeviceId(retrieveObj, strdup(deviceID));
    }

    if(sensorID) {
        addSensorId(retrieveObj, strdup(sensorID));
    }

    response = retrieveData2(retrieveObj);

    return response;
}

/**
 * @name Subscribe to a topic
 * @brief Registers the client's callback to be invoked on receiving a message from MQTT broker.
 * @param handler to be registered as a callback
 */
int receive(void (*handler) (char *topic, Context context)) {
    int rc = 0;
    char uri[256];
    int i = 0;
    long currentTime;
    long previousTime;
    Context context;
    char *response;

    initializeDeviceCredentials();

    context.name = NULL;
    context.value = NULL;

    currentTime = previousTime = 0;

    while(1) {
        if(currentTime == 0) {
            currentTime = getCurrentTimeInSeconds();
            previousTime = currentTime - (frequencyInterval * 1000);
        } else {
            previousTime = currentTime + 1;
            currentTime = getCurrentTimeInSeconds();
        }

        response = retrieve(sensorName, targetDeviceId, previousTime, currentTime);
        handler(response, context);
        sleep(frequencyInterval);
    }

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

        return response;
    }

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
    #if DEBUG
        printf("Invoked init()\n");
    #endif

    ServiceQuery *serviceQuery = (ServiceQuery *) servQuery;

    activationCode = serviceQuery->type_params.activationCode;
    deviceID = serviceQuery->type_params.deviceid;
    address = serviceQuery->address;
    port = serviceQuery->port;
    parseServiceName(serviceQuery->service_name);

    targetDeviceId = serviceQuery->type_params.subscribeToDevice;
    frequencyInterval = serviceQuery->type_params.frequencyInterval;

    if(frequencyInterval <= 0) {
        frequencyInterval = 5; //default 5 seconds
    }

    iotkit_init();

    activateDevice();

    return 0;
}
