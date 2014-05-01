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



#include "edisonapi.h"

int init(void *serviceDesc);
int sendTo(void *,char *,Context context);	// int send(client, message, context) // for example, incase of mqtt... int sendTo(<<mqtt client>>, message, context);
int publish(char *,Context context); // int publish(message,context)
int manageClient(void *,Context context); // int manageClient(client,context) // for example, incase of mqtt... int manageClient(<<mqtt client>>, context);
int receive(void (*)(void *client,char *message,Context context)); // int receive(handler) // handler takes 3 parameters
int done();