/*
 * Edison 'C' Library to load plugins on-demand
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

#define FALSE 0
#define TRUE 1

typedef struct _Context {
    char *name;
    char *value;
} Context;

// handle to the client communication plugin
typedef struct _CommClientHandle {
    char **interface; // filename for pluing-interface json file
    int (*init)(void *);
    int (*send)(char *, Context context);	// int send(topic, context)
    int (*subscribe)(char *);	// int subscribe(topic)
    int (*unsubscribe)(char *);	// int unsubscribe(topic)
    int (*receive)(void (*)(char *, Context)); // int setReceivedMessageHandler(handler) // handler takes 2 parameters
    int (*done)();
    void *handle;	// handle to the dll
} CommClientHandle;

// handle to the service communication plugin
typedef struct _CommServiceHandle {
    char **interface; // filename for pluing-interface json file
    int (*init)(void *);
    int (*sendTo)(void *, char *, Context context);	// int send(client, message, context) // for example, incase of mqtt... int sendTo(<<mqtt client>>, message, context);
    int (*publish)(char *,Context context); // int publish(message,context)
    int (*manageClient)(void *,Context context); // int manageClient(client,context) // for example, incase of mqtt... int manageClient(<<mqtt client>>, context);
    int (*receive)(void (*)(void *, char *, Context context)); // int setReceivedMessageHandler(handler) // handler takes 3 parameters
    int (*done)();
    void *handle;	// handle to the dll
} CommServiceHandle;


//typedef enum { ADDED, REMOVED, UNKNOWN/home/skothurx/mango/edison-api/src-c/edison-lib/libedison/plugin-interfaces } ServiceStatus;

typedef struct _Prop {
    char *key;
    char *value;
} Property;

// service description
typedef struct _ServiceDescription {
    enum { ADDED, REMOVED, REGISTERED, IN_USE, UNKNOWN } status;
    char *service_name;	    // name of the service
    struct {
	char *name;
	char *protocol; // the protocol
    } type;
    char *address;
    int port;
    struct {
        char *ssl;
    } comm_params;
    int numProperties;
    Property *properties;
    struct {
        char *locally;
        char *cloud;
    } advertise;
} ServiceDescription, ServiceQuery;

// Create client which returns a CommClientHandle
CommClientHandle *createClient(ServiceQuery *);

// Create service which returns a CommServiceHandle
CommServiceHandle *createService(ServiceDescription *);


void cleanUpClient(CommClientHandle *);
void cleanUpService(CommServiceHandle *);
