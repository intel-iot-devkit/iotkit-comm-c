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

/** @file test_edisonapi_parseplugininterfaces_success.c

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
    if (parsePluginInterfaces("../../../src-c/edison-lib/libedison/plugin-interfaces/edison-client-interface.json")) {
        puts("\nSuccess: Parsed Config File\n");
        exit(EXIT_SUCCESS);
    } else {
        puts("\nFailed: Parsing Config File\n");
        exit(EXIT_FAILURE);
    }
}