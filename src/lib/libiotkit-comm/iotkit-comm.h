/*
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
* @file iotkit-comm.h
* @brief Header file of iotkit-comm Library.
*
* Data Structure of Service Specification, Service Query, Communication Handle and Context.
*/

#ifndef __IOTKIT_COMM_H
#define __IOTKIT_COMM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <pwd.h>
#include <regex.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#ifndef DEBUG
    #define DEBUG 0
#endif

#define READ 0
#define WRITE 1

#ifndef LIB_CONFIG_DIRECTORY
    #define LIB_CONFIG_DIRECTORY "/etc/iotkit-comm/"
#endif

#ifndef LIB_CONFIG_FILENAME
    #define LIB_CONFIG_FILENAME "config.json"
#endif

#ifndef USER_CONFIG_FILENAME
    #define USER_CONFIG_FILENAME ".iotkit-comm_config.json"
#endif

#ifndef LIB_PLUGINS_DIRECTORY
    #define LIB_PLUGINS_DIRECTORY "/usr/lib/"
#endif

typedef struct _AuthenticationState {
    char *user;
    char *host;
    int mosquittoSecurePort;
} AuthenticationState;

/** System level Configuration data read from the config JSON.
*/
typedef struct _ConfigFileData {
    char *pluginInterfaceDir; // plugin interface directory
    char *pluginDir; // plugin directory
    char *clientFileSuffix; // client plugin suffix
    char *serverFileSuffix; // service plugin suffix
    AuthenticationState *localState;
    AuthenticationState *globalState;

    int unusedPortDetectAttempts;
    char *stateDirName;
    char *stateMapName;
    char *localAddr;
    int portMin;
    int portMax;

    // authentication specific parameters
    char *keyDirSuffix;
    char *keyDirName;
    char *caName;
    char *privateKeyNameSuffix;
    char *hostKeyNameSuffix;
    char *clientKeyNameSuffix;
    char *SSLCertSuffix;
    char *SSLCertReqSuffix;
    char *SSHPubKeySuffix;
    char *defaultUser;
    char *SSHCertSuffix;
} ConfigFileData;

/** Context to be passed around the callback methods as a name-value pair.
*/
typedef struct _Context {
    char *name;
    char *value;
} Context;

typedef struct _Interfaces {
    char *iname; // interface name
    void *iptr; // interface or function address
} Interfaces;

/** Handle to the communication plugin.
*/
typedef struct {
    char **interface; // specifies the filename for plugin-interface json file
    bool *provides_secure_comm;
    bool *communicates_via_proxy;

    int (*init)(void *, void *); // initializes the plugin

    Interfaces **interfaces;
    int interfacesCount;
    void *handle; // handle to the library
} CommHandle;

/** Property stored as key-value pair.
*/
typedef struct _Prop {
    char *key;
    char *value;
} Property;

/** Service specification and query.
 */
typedef struct _ServiceSpec {
    enum { ADDED, REMOVED, REGISTERED, IN_USE, UNKNOWN } status; // current status of the service/client
    char *service_name; // name of the service
    struct {
        char *name; // serive type name
        char *protocol; // service protocol
    } type;
    char *address; // address where service is available
    int port; // port at which service is running
    int commParamsCount; // count of comm params
    Property **comm_params; // list of comm params
    int numProperties; // count of properties
    Property **properties; // list of properties
    struct {
        char *locally;
        char *cloud;
    } advertise;
    struct {
        bool mustsecure;
        char *deviceid;
    } type_params;
} ServiceSpec,ServiceQuery;


#define SSH_TUNNEL_CMD_SUCCESS_RE_STRING "debug1: channel .+: new";
#define SSH_TUNNEL_ADDRESS_IN_USE_RE_STRING "bind: Address already in use";


/** holds authentication details
*/
typedef struct _Crypto {
    char *user;
    char *host;
    int mosquittoSecurePort;

    char cacert[12288];
    char hostpubkey[1024];
    char hostkey[1024];
    char hostsshcert[12288];
    char hostsslcert[12288];
    char userkey[1024];
    char userpubkey[1024];
    char usersshcert[12288];
    char usersslcert[12288];

    pid_t tunnelproc;
    bool destroyingTunnel;
    int portInUse;
    char *successRE;
    char *portInUseRE;
} Crypto;


Crypto *gCrypto;

ConfigFileData g_configData;

/** list of function signatures specified in the interface JSON
*/
char **g_funcSignatures;
int g_funcEntries;

CommHandle *createClient(ServiceQuery *);
CommHandle *createService(CommHandle *commHandle, ServiceSpec *specification);
void *commInterfacesLookup(CommHandle *commHandle, char *funcname);
void cleanUp(CommHandle **);
void cleanUpService(ServiceSpec **servSpec, CommHandle **commHandle);
char *getSpecPropertyValue(ServiceSpec *specification, char *key);
bool fileExists(char *absPath);

Crypto *crypto_init();
void addSpecProperty(ServiceSpec *specification, char *key, char *value);
bool createSecureTunnel(ServiceSpec *specification, int *localport, char **localaddr);

#endif
