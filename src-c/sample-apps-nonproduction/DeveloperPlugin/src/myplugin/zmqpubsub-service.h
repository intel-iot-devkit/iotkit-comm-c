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

/** @file zmqpubsub-service.h

    Header file. This class lists all those functions in zmq pub/sub service.

 */

#include "edisonapi.h"

/** @defgroup zmqpubsubservice
*
*  @{
*/

char *interface = "myplugin-service-interface"; /**< specifies the plugin interface json */

int init(void *publishServiceDesc);
int publish(char *message,Context context);
int done();

/** @} */ // end of zmqpubsubservice