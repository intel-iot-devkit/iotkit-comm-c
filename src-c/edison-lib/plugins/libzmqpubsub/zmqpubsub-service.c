/*
 * ZMQ PUB/SUB plugin to enable publish feature through Edison API
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

/** @file zmqpubsub-service.c
    Provides functions to publish message to clients who are connected and subscribed a topic.
*/

#include "zmqpubsub-service.h"

/** @defgroup zmqpubsubservice
*   This is ZMQ publish service.
*   @{
*/

/** Creates the context object and publisher socket. With the help of the ServiceDescription parameter, the publisher
socket binds to the address and port to initiate communication.
* @param publishServiceDesc an void pointer
* @return The result code
*/
int init(void *publishServiceDesc) {
    #if DEBUG
        printf("In createService\n");
    #endif
    ServiceDescription *serviceDescription = (ServiceDescription *) publishServiceDesc;
    zmqContainer.context = zmq_ctx_new();
    #if DEBUG
        printf("context initialised\n");
    #endif
    zmqContainer.publisher = zmq_socket(zmqContainer.context,ZMQ_PUB);
    char addr[128];
    if (serviceDescription->address != NULL) {
        sprintf(addr, "tcp://%s:%d", serviceDescription->address, serviceDescription->port);
    } else {
        sprintf(addr, "tcp://*:%d", serviceDescription->port);
    }
    #if DEBUG
        printf("going to bind %s\n",addr);
    #endif
    int rc = zmq_bind(zmqContainer.publisher,addr);
    #if DEBUG
        printf("bind completed\n");
    #endif

    return rc;
}

int sendTo(void *client, char *message, Context context) {
    #if DEBUG
        printf("In sendTo\n");
    #endif
}

/**  Publishing a message. This function will publish message to the clients.
* @param message a string message
* @param context a context object
* @return The result code
*/
int publish(char *message,Context context) {
    #if DEBUG
        printf("publish started\n");
    #endif
    #if DEBUG
        printf("Publishing message %s\n",message);
    #endif
    int rc = s_send(zmqContainer.publisher, message);
    #if DEBUG
        printf("Publisher Finished\n");
    #endif
    return rc;
}

int manageClient(void *client,Context context) {
    #if DEBUG
        printf("In manageClient\n");
    #endif
}

int receive(void (*publishServiceHandler) (void *client,char *message,Context context)) {
    #if DEBUG
        printf("In receive\n");
    #endif
}

/** Cleanup function. This function close the publisher socket and destroy the context object.
* @return The result code
*/
int done() {
    if (zmqContainer.publisher != NULL) {
        zmq_close(zmqContainer.publisher);
        zmqContainer.publisher = NULL;
        #if DEBUG
            printf("publisher freed\n");
        #endif
    }
    if (zmqContainer.context != NULL) {
        zmq_ctx_destroy(zmqContainer.context);
        zmqContainer.context = NULL;
        #if DEBUG
            printf("context freed\n");
        #endif
    }
    #if DEBUG
        printf("\nclosed\n");
    #endif
}

/** @} */ // end of zmqpubsubservice
