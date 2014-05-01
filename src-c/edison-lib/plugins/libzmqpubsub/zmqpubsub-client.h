/*
 * ZMQ PUB/SUB plugin to enable subscribe feature through Edison API
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

int init(void *serviceQuery);
int send(char *, Context);	// int send(message, context)
int subscribe(char *);	// int subscribe(topic)
int unsubscribe(char *);	// int unsubscribe(topic)
int receive(void (*)(char *, Context)); // int receive(handler) // handler takes 2 parameters
int done();
