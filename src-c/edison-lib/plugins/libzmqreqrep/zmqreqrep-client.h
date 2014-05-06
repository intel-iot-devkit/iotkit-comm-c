/*
 * ZMQ REQ/REP plugin to enable request feature through Edison API
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

/** @file zmqreqrep-client.h

    Header file. This file lists all those functions which helps in implementing ZMQ REQ/REP

 */

#include "edisonapi.h"

/** @defgroup zmqreqrepclient
*
*  @{

*/
int init(void *serviceQuery);
int send(char *message, Context);
int subscribe(char *topic);
int unsubscribe(char *topic);
int receive(void (*handler)(char *message, Context context));
int done();
/** @} */ // end of zmqreqrepclient