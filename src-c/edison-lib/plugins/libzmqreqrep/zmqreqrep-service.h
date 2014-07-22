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

/** @file zmqreqrep-service.h
    Header file. This file lists all those functions which helps in implementing ZMQ REQ/REP
*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../inc/zhelpers.h"

#include "edisonapi.h"

#ifndef DEBUG
    #define DEBUG 0
#endif

/** @defgroup zmqreqrepservice
*
*  @{
*/
char *interface = "edison-service-interface"; /**< specifies the plugin interface json */

/** Structure which holds the context and responder handler
*/
struct ZMQReqRepService {
    void *context; /**< context handler */
    void *responder; /**< responder handler */
};

/** An Global ZMQReqRepService Object.
*/
struct ZMQReqRepService zmqContainer;

int init(void *responseServiceDesc);
int sendTo(void *client,char *message,Context context);
int publish(char *message,Context context);
int manageClient(void *client,Context context);
int receive(void (*responseServiceHandler)(void *client,char *message,Context context));
int done();
/** @} */ // end of zmqreqrepservice
