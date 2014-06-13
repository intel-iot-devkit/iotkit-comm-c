/*
 * Edison API
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

/** @file test_edisonapi_createclient_success.c

*/

#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <stdlib.h>
#include <signal.h>
#include "../../edison-lib/libedison/edisonapi.h"

int main (void)
{
    ServiceQuery *query = (ServiceQuery *) parseClientServiceQuery("../../../src-c/tests/libedison/temperatureServiceQueryZMQPUBSUB.json");
    if (query && createClient(query)) {
         puts("\nSuccess: Created Client\n");
         exit(EXIT_SUCCESS);
    } else {
         puts("\nFailed: Create Client\n");
         exit(EXIT_FAILURE);
    }
}