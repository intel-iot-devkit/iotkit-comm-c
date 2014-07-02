/*
 * MDNS through Edison API
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

/** @file test_mdns_discoverservice_success.c

*/

#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <stdlib.h>
#include "../../edison-lib/libedison/edisonapi.h"


/** Callback function. Once the service is discovered this callback function will be invoked
* @param queryDesc the query description object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */
void subDiscoveryCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle)
{

    if (commHandle != NULL) {
        puts("\nSuccess: Service Found\n");
        exit(EXIT_SUCCESS);
    } else {
        puts("\nFail: Service Not Found\n");
        exit(EXIT_FAILURE);
    }
}

int main (void)
{

    ServiceQuery *query = (ServiceQuery *) parseClientServiceQuery("./temperatureServiceQueryMQTT.json");
    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./temperatureServiceMQTT.json");

    if (serviceDescription) {
	    if (query && advertiseService(serviceDescription)) {
            fprintf(stderr,"query host address %s\n",query->address);
            fprintf(stderr,"query host port %d\n",query->port);
            fprintf(stderr,"query service name %s\n",query->service_name);
            WaitToDiscoverServices(query, subDiscoveryCallback);
        }
    }
    return 0;
}