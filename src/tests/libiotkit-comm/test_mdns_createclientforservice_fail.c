/*
 * MDNS through iotkit-comm API
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

/** @file test_mdns_createclientforservice_fail.c

*/

#include <stdio.h>
#include <zmq.h>
#include <zmq_utils.h>
#include "../../lib/libiotkit-comm/iotkit-comm.h"


/** Callback function. Once the service is discovered this callback function will be invoked
* @param queryDesc the query description object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void callback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *clientHandle) {
     if (clientHandle != NULL) {
         puts("Success: Service Advertised");
         exit(EXIT_FAILURE);
     } else {
         puts("Fail: Service Not Advertised");
         exit(EXIT_SUCCESS);
     }
}

int main(void) {
    ServiceQuery *query = (ServiceQuery *)parseClientServiceQuery("./temperatureServiceQueryMQTT.json");
    if (query) {
        createClientForGivenService(query, callback);
    }
    return 0;
}
