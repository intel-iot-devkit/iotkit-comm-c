/*
* ZMQ REQ/REP plugin to enable respond feature through Edison API
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

/** @file zmqreqrep-service.c
    This class provides functions to send and receive message to/from a Client to which it
    is connected to.
*/

#include "zmqreqrep-service.h"

/** @defgroup zmqreqrepservice
* This is ZMQ REQ REP Service
* @{
*/

/** Creates the context object and responder socket. With the help of the ServiceDescription parameter the responder
socket binds connection to the address and port to initiate communication.
* @param responseServiceDesc an void pointer
* @return The result code
*/
int init(void *responseServiceDesc) {
    #if DEBUG
        printf("context initialised\n");
    #endif
    ServiceDescription *serviceDescription = (ServiceDescription *)responseServiceDesc;
    zmqContainer.context = zmq_ctx_new();

    // This is server side
    char addr[128];
    if (serviceDescription->address != NULL) {
        sprintf(addr, "tcp://%s:%d", serviceDescription->address, serviceDescription->port);
    } else {
        sprintf(addr, "tcp://*:%d", serviceDescription->port);
    }

    #if DEBUG
        printf("going to bind %s\n",addr);
    #endif
    zmqContainer.responder = zmq_socket(zmqContainer.context,ZMQ_REP);
    int rc = zmq_bind(zmqContainer.responder,addr);
    #if DEBUG
        printf("bind completed\n");
    #endif
    return rc;
}

/** Sending a message. The service can send a message to the client to which it is connected to.
* @param client a client object
* @param message a string message
* @param context a context message
* @return The result code
*/
int sendTo(void *client,char *message,Context context) {
    #if DEBUG
        printf ("Sending message from Service %s...\n", message);
    #endif
    int rc = s_send (zmqContainer.responder, message);
    return rc;
}

/** Empty function. This function is unimplemented since in ZMQ req/rep this is not applicable
* @param message a string message
* @param context a context object
* @return The result code
*/
int publish(char *message,Context context) {
    #if DEBUG
        printf("In publish\n");
    #endif
}

/**  Empty function. This function is unimplemented since in ZMQ req/rep this is not applicable
* @param client a client object
* @param context a context object
* @return The result code
*/
int manageClient(void *client,Context context) {
    #if DEBUG
        printf ("In manageClient\n");
    #endif
}

/** Receive the message. The service will be receiving an handler which is used as a callback mechanism to pass the
received message.
* @param responseServiceHandler a callback handler which takes a client,message,context object as params
* @return The result code
*/
int receive(void (*responseServiceHandler)(void *client,char *message,Context context)) {
    #if DEBUG
        printf("In receive\n");
    #endif
    void *client;
    int rc = 0;
    //  Read message contents
    char *message = s_recv (zmqContainer.responder);
    #if DEBUG
        printf ("Received message in service: %s\n", message);
    #endif
    if (message == NULL) {
        rc = -1;
    }
    Context context;
    context.name = "event";
    context.value = "message";
    responseServiceHandler(client,message,context);
    free(message);
}

/** Cleanup function. This method helps in closing the responder socket and destroying the
context object.
* @return The result code
*/
int done() {
    int rc = -1;
    if (zmqContainer.responder != NULL) {
        rc = zmq_close(zmqContainer.responder);
        zmqContainer.responder = NULL;
        #if DEBUG
            printf("responder freed\n");
        #endif
    }
    if (zmqContainer.context != NULL) {
        rc = zmq_ctx_destroy(zmqContainer.context);
        zmqContainer.context = NULL;
        #if DEBUG
            printf("context freed\n");
        #endif
    }
    #if DEBUG
        printf("\nclosed\n");
    #endif
    return rc;
}
/** @} */ // end of zmqreqrepservice
