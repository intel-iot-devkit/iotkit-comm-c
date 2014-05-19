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

#ifndef __EDISON_API_H__
#define __EDISON_API_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <pwd.h>

#include <cJSON.h>

#include "util.h"


/** System level Configuration data read from the config JSON
 */
typedef struct _ConfigFileData {
    char *pluginInterfaceDir; // plugin interface directory
    char *pluginDir; // plugin directory
    char *clientFileSuffix; // client plugin suffix
    char *serverFileSuffix; // service plugin suffix
    char *plugin; // plugin directory where the plugin is present
} ConfigFileData;

/** Context to be passed around the callback methods as a name value pair
 */
typedef struct _Context {
    char *name;
    char *value;
} Context;


typedef struct _Interfaces {
    char *iname; // interface name
    void *iptr; // interface or function address
} Interfaces;

/** Handle to the communication plugin
 */
typedef struct {
    char **interface; // specifies the filename for plugin-interface json file

    int (*init)(void *); // initializes the plugin

    Interfaces **interfaces;
    int interfacesCount;
    void *handle;	// handle to the library
} CommClientHandle, CommServiceHandle;


//typedef enum { ADDED, REMOVED, UNKNOWN/home/skothurx/mango/edison-api/src-c/edison-lib/libedison/plugin-interfaces } ServiceStatus;

/** Property stored as key value pair
 */
typedef struct _Prop {
    char *key;
    char *value;
} Property;


/** service description
 */
typedef struct _ServiceDescription {
    enum { ADDED, REMOVED, REGISTERED, IN_USE, UNKNOWN } status; // current status of the service/client
    char *service_name;	    // name of the service
    struct {
	char *name; // serive type name
	char *protocol; // service protocol
    } type;
    char *address; // address where service is available
    int port; // port at which service is running
    struct { // TODO: comm_params should hold key value pairs
        char *ssl;
    } comm_params;
    int numProperties; // count of properties
    Property **properties; // list of properties
    struct {
        char *locally;
        char *cloud;
    } advertise;
} ServiceDescription, ServiceQuery;


ConfigFileData g_configData;

/** list of function signatures specified in the interface JSON
 */
char **g_funcSignatures;
int g_funcEntries;



CommClientHandle *createClient(ServiceQuery *);
CommServiceHandle *createService(ServiceDescription *);

void cleanUpClient(CommClientHandle *);
void cleanUpService(CommServiceHandle *);

bool fileExists(char *absPath);

#endif