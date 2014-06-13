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

/** @file test_zmqreqrep_rep_socket_connect_success.c

This file tests whether ZMQ Responder socket is able to successfully connect.
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
    serviceDesc->port = 1080;
    int result = init(serviceDesc);
    done();
    free(serviceDesc);
    if (result == 0) {
        printf("Publisher Socket Successfully Binded\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("Failed: Publisher Socket Binding\n");
        exit(EXIT_FAILURE);
    }
}