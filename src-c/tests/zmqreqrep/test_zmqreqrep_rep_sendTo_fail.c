/*
 * ZMQ REQ/REP plugin through Edison API
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

/** @file test_zmqreqrep_rep_sendTo_fail.c

This file tests whether ZMQ Responder socket fails while sending message.
*/

#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../edison-lib/libedison/edisonapi.h"

int main (void)
{

    ServiceDescription *serviceDesc = (ServiceDescription *)malloc(sizeof(ServiceDescription));
    serviceDesc->address = "127.0.0.1";
    serviceDesc->port = 123423;
    init(serviceDesc);
    done();
    free(serviceDesc);
    int result = sendTo(NULL,"Hello World",NULL);
    if (result == 0) {
        printf("Sended Message Successfully\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Failed: Sending Message\n");
        exit(EXIT_SUCCESS);
    }
}