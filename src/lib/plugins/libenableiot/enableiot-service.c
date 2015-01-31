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
    char *name, *latitude, *longitude, *height;
    char valuestr[256];
    char *out;
    int value;

    name = latitude = longitude = height = NULL;

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

        jitem = cJSON_GetObjectItem(json, "latitude");
        if (jitem && isJsonString(jitem)) {
            latitude = strdup(jitem->valuestring);
            #if DEBUG
                printf("latitude value = %s\n", latitude);
            #endif
        }

        jitem = cJSON_GetObjectItem(json, "longitude");
        if (jitem && isJsonString(jitem)) {
            longitude = strdup(jitem->valuestring);
            #if DEBUG
                printf("longitude value = %s\n", longitude);
            #endif
        }

        jitem = cJSON_GetObjectItem(json, "height");
        if (jitem && isJsonString(jitem)) {
            height = strdup(jitem->valuestring);
            #if DEBUG
                printf("height value = %s\n", height);
            #endif
        }

        submitData(name, valuestr, latitude, longitude, height);

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

//    iotkit_cleanup();

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

void initializeDeviceProperties() {
    char *config_path = "/usr/share/iotkit-lib/device_config.json";
    cJSON *json = NULL, *jitem = NULL;
    FILE *fp = NULL;
    char *out;

    deviceID = data_account_name = NULL;

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

            if (isJsonObject(json)) {
                jitem = cJSON_GetObjectItem(json, "deviceId");
                if (isJsonString(jitem)) {
                    deviceID = strdup(jitem->valuestring);
                    #if DEBUG
                        printf("deviceID = %s\n", deviceID);
                    #endif
                }

                jitem = cJSON_GetObjectItem(json, "data_account_name");
                if (isJsonString(jitem)) {
                    data_account_name = strdup(jitem->valuestring);
                    #if DEBUG
                        printf("data_account_name = %s\n", data_account_name);
                    #endif
                }

                cJSON_Delete(json);
            }
        }

        free(buffer);
        fclose(fp);
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
    ServiceQuery *serviceQuery = (ServiceQuery *) servQuery;
    #if DEBUG
        printf("Invoked init()\n");
    #endif

//    iotkit_init();

    parseServiceName(serviceQuery->service_name);

//    initializeDeviceProperties();
//    iotkit_cleanup();

    return 0;
}

char *signIn(char *username, char *passwd) {
    bool isActivated = false;

    iotkit_init();
    initializeDeviceProperties();
    isActivated = isDeviceActivated(); // if device is already active; no need to generate new authentication token

    if(!isActivated) {
        char *response = NULL;
        response = getUserJwtToken(username, passwd);
        return response;
    }

    iotkit_cleanup();

    return NULL;
}

char *createAccount(char *accname) {

    iotkit_init();
    initializeDeviceProperties();
    if(data_account_name == NULL || strcmp(accname, data_account_name) != 0) {
        char *response = NULL;
        response = createAnAccount(accname);
        return response;
    }

    iotkit_cleanup();

    return NULL;
}

char *createDevice(char *deviceid, char *gatewayid, char *devicename) {
    bool isActivated = false;

    iotkit_init();
    initializeDeviceProperties();
    isActivated = isDeviceActivated(); // if device is already active; no need to create again

    if(!isActivated) {
        char *response = NULL;
        DeviceCreationObj *createDeviceObj = createDeviceCreationObject(deviceid, gatewayid, devicename);
//        addLocInfo(createDeviceObj, strdup("74.4352"), strdup("34.0823"), strdup("78.03"));
//        addTagInfo(createDeviceObj, strdup("tagname1"));
//        addAttributesInfo(createDeviceObj, strdup("attrname"), strdup("attrvalue"));
        response = createADevice(createDeviceObj);

        return response;
    }

    iotkit_cleanup();

    return NULL;
}

char *activateDevice() {
    bool isActivated = false;

    iotkit_init();
    initializeDeviceProperties();
    isActivated = isDeviceActivated(); // if device is already active; no need to activate again

    if(!isActivated) {
        char *response = NULL;
        cJSON *json = NULL, *child = NULL, *jitem = NULL;
        char *actCode = NULL, *out = NULL;
        char *actCodeResponse = renewActivationCode();

        #if DEBUG
            printf("Activation Request response is %s\n", actCodeResponse);
        #endif

        json = cJSON_Parse(actCodeResponse);

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
                goto endParseStateConfig2;
            }

            child = cJSON_GetObjectItem(json, "data");
            if (!isJsonObject(json)) {
                fprintf(stderr,"Invalid JSON format\n");
                goto endParseStateConfig2;
            }

            jitem = cJSON_GetObjectItem(child, "activationCode");
            if (!isJsonString(jitem)) {
                fprintf(stderr,"Invalid JSON format\n");
                goto endParseStateConfig2;
            }

            actCode = strdup(jitem->valuestring);
            #if DEBUG
                printf("Activation Code = %s\n", actCode);
            #endif

endParseStateConfig2:
            cJSON_Delete(json);
        }

        response = activateADevice(actCode);

        return response;
    }

    iotkit_cleanup();

    return NULL;
}

/**
 * @name Registers a sensor
 * @brief Registers a sensor with IoT Cloud
 */
char *registerSensor() {
    char *response = NULL;

    iotkit_init();
    initializeDeviceProperties();
    response = addComponent(sensorName, sensorType);

    iotkit_cleanup();

    return response;
}
