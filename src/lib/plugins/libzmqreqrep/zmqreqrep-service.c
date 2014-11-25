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

/** @file zmqreqrep-service.c
    Provides functions to send and receive message to/from a Client it is connected to.
*/

#include "zmqreqrep-service.h"

/** @defgroup zmqreqrepservice
* This is ZMQ replier service
* @{
*/

/** Creates the context object and responder socket. With the help of the ServiceSpec parameter, the responder
socket binds, connection to the address and port to initiate communication.
* @param responseServiceDesc an void pointer
* @return The result code
*/
int init(void *responseServiceDesc, Crypto *crypto) {
    #if DEBUG
        printf("context initialised\n");
    #endif
    ServiceSpec *serviceSpecification = (ServiceSpec *)responseServiceDesc;
    zmqContainer.context = zmq_ctx_new();

    // This is server side
    char addr[128];
    if (serviceSpecification->address != NULL) {
        sprintf(addr, "tcp://%s:%d", serviceSpecification->address, serviceSpecification->port);
    } else {
        sprintf(addr, "tcp://*:%d", serviceSpecification->port);
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

/** Sending a message. The service can send a message to the client it is connected to.
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

int publish(char *message,Context context) {
    #if DEBUG
        printf("In publish\n");
    #endif
    return -1;
}

int manageClient(void *client,Context context) {
    #if DEBUG
        printf ("In manageClient\n");
    #endif
    return -1;
}

/** Receive the message. The parameter in this function is used as a callback mechanism to pass the
received message.
* @param responseServiceHandler a callback handler which takes a client,message,context object as params
* @return The result code
*/
int receive(void (*responseServiceHandler)(void *clnt,char *mesg,Context ctx)) {
    #if DEBUG
        printf("In receive\n");
    #endif
    void *client = NULL;
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
    return rc;
}

/** Cleanup function. This function close the responder socket and destroy the context object.
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
