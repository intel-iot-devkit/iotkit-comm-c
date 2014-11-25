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
* @file iotkit-comm.c
* @brief Implementation of iotkit-comm Library.
 *
 * Provides functions to create a client and service, to load communication interfaces and to load the communication
   plugins.
 */

#include "iotkit-comm.h"

#include "cJSON.h"
#include "util.h"

#define MAX_PROPERTIES 128

/** Reports dynamic link library errors. Checks for any recent error while loading shared library.
 * @return boolean specifies whether an error was present or not
 */
static inline bool checkDLError() {
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "DL error %s\n", error);
        return false;
    }
    return true;
}

/*
* Helper snippet to print JSON parse errors.
*/
#define handleParseConfigError() \
{\
    status = false;\
    fprintf(stderr,"invalid JSON format for %s file\n", config_file);\
    goto endParseConfig;\
}

#define handleUserDefinedConfigError() \
{\
    fprintf(stderr,"invalid JSON format for %s file\n", config_file);\
    goto endUserDefinedConfig;\
}

/** Considers user-defined configuration. Loads and parses user-defined configuration
* '.config.json' present in user's home directory for 'plugin interface' and
* 'plugins' configuration properties and concats with iotkit-comm configuration properties.
*/
void concatUserDefinedConfigurations() {
    char *home, config_file[1024];

    home = getenv("HOME");
    if (home == NULL || strlen(home) == 0) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
    }

    strcpy(config_file, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(config_file, "/");
    }

    strcat(config_file, USER_CONFIG_FILENAME);

    char *out;
    cJSON *json = NULL, *jitem;
    FILE *fp = fopen(config_file, "rb");

    if (fp == NULL) {
        fprintf(stderr,"Warning: Optional user config file not found. Continuing...\n");
    } else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
        } else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                fprintf(stderr,"%s\n", out);
                free(out);
                out = NULL;
            #endif

            if (!isJsonObject(json)) {
                handleUserDefinedConfigError();
            }

            jitem = cJSON_GetObjectItem(json, "pluginInterfaceDirPaths");
            if (!isJsonString(jitem)) {
                handleUserDefinedConfigError();
            }

            int pathSize = strlen(g_configData.pluginInterfaceDir) + strlen(jitem->valuestring) + 2;
            char *pluginInterfaceDirPaths = (char *)malloc(sizeof(char) * pathSize);
            if (pluginInterfaceDirPaths != NULL) {
                strcpy(pluginInterfaceDirPaths, g_configData.pluginInterfaceDir);
                strcat(pluginInterfaceDirPaths, ":");
                strcat(pluginInterfaceDirPaths, jitem->valuestring);
                g_configData.pluginInterfaceDir = pluginInterfaceDirPaths;
            }
            #if DEBUG
                printf("pluginInterfaceDir = %s\n", g_configData.pluginInterfaceDir);
            #endif

            jitem = cJSON_GetObjectItem(json, "pluginDirPaths");
            if (!isJsonString(jitem)) {
                handleUserDefinedConfigError();
            }

            pathSize = strlen(g_configData.pluginDir) + strlen(jitem->valuestring) + 2;
            char *pluginDirPaths = (char *)malloc(sizeof(char) * pathSize);
            if (pluginDirPaths != NULL) {
                strcpy(pluginDirPaths, g_configData.pluginDir);
                strcat(pluginDirPaths, ":");
                strcat(pluginDirPaths, jitem->valuestring);
                g_configData.pluginDir = pluginDirPaths;
            }
            #if DEBUG
                printf("pluginDir = %s\n", g_configData.pluginDir);
            #endif

endUserDefinedConfig:
            cJSON_Delete(json);
        }

        free(buffer);
        fclose(fp); // close the file pointer
    }
}

char *getGlobalStateLoc() {
    // TODO: support for different platforms
    return strdup("/usr/local");
}

AuthenticationState *readAuthenticationState(char *config_file) {
    cJSON *json = NULL, *jitem = NULL;
    AuthenticationState *authenticationState = NULL;
    FILE *fp = NULL;
    char *out;

    fp = fopen(config_file, "rb");

    if (fp == NULL) {
        fprintf(stderr,"Error can't open file %s\n", config_file);
    } else {
        // TODO: export the local state
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
        } else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                fprintf(stderr,"%s\n", out);
                free(out);
                out = NULL;
            #endif

            authenticationState = (AuthenticationState *)malloc(sizeof(AuthenticationState));
            authenticationState->host = NULL;
            authenticationState->user = NULL;
            authenticationState->mosquittoSecurePort = 0;

            if (!isJsonObject(json)) {
                fprintf(stderr,"invalid JSON format for %s file\n", config_file);
                goto endParseStateConfig;
            }

            jitem = cJSON_GetObjectItem(json, "host");
            if (!isJsonString(jitem)) {
                fprintf(stderr,"invalid JSON format for %s file\n", config_file);
                goto endParseStateConfig;
            }

            authenticationState->host = strdup(jitem->valuestring);
            #if DEBUG
                printf("state->host = %s\n", authenticationState->host);
            #endif

            jitem = cJSON_GetObjectItem(json, "user");
            if (jitem && isJsonString(jitem)) {
                authenticationState->user = strdup(jitem->valuestring);
                #if DEBUG
                    printf("state->user = %s\n", authenticationState->user);
                #endif
            } else {
                authenticationState->user = NULL;
            }

            jitem = cJSON_GetObjectItem(json, "mosquittoSecurePort");
            if (jitem && isJsonNumber(jitem)) {
                authenticationState->mosquittoSecurePort = jitem->valueint;
                #if DEBUG
                    printf("state->mosquittoSecurePort = %d\n", authenticationState->mosquittoSecurePort);
                #endif
            }

endParseStateConfig:
            cJSON_Delete(json);
        }

        free(buffer);
        fclose(fp);

        return authenticationState;
    }

    return NULL;
}

void loadLocalState() {
    char *home, config_file[1024];

    home = getenv("HOME");
    if (home == NULL || strlen(home) == 0) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
    }

    strcpy(config_file, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(config_file, "/");
    }

    strcat(config_file, ".");
    strcat(config_file, g_configData.stateDirName);
    strcat(config_file, "/");
    strcat(config_file, g_configData.stateMapName);

    g_configData.localState = readAuthenticationState(config_file);
}

void loadGlobalState(char *location) {
    char *home, config_file[1024];

    strcpy(config_file, location);
    if(*(location + (strlen(location) -1)) != '/') {
        strcat(config_file, "/");
    }
    strcat(config_file, g_configData.stateDirName);
    strcat(config_file, "/");
    strcat(config_file, g_configData.stateMapName);

    g_configData.globalState = readAuthenticationState(config_file);
}

/** Parses iotkit-comm configuration file
 * @param[in] config_file path to the configuration file
 */
bool parseConfigFile(char *config_file) {
    char *out;
    cJSON *json = NULL, *jitem, *child, *subjson;
    bool status = true;
    FILE *fp = fopen(config_file, "rb");

    if (fp == NULL) {
        fprintf(stderr,"Error can't open file %s\n", config_file);
        status = false;
    } else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
            status = false;
        } else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                fprintf(stderr,"%s\n", out);
                free(out);
                out = NULL;
            #endif

            if (!isJsonObject(json)) {
                handleParseConfigError();
            }

            jitem = cJSON_GetObjectItem(json, "pluginInterfaceDir");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.pluginInterfaceDir = strdup(jitem->valuestring);
            #if DEBUG
                printf("pluginInterfaceDir = %s\n", g_configData.pluginInterfaceDir);
            #endif

            jitem = cJSON_GetObjectItem(json, "pluginDir");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.pluginDir = strdup(jitem->valuestring);
            #if DEBUG
                printf("pluginDir = %s\n", g_configData.pluginDir);
            #endif


            jitem = cJSON_GetObjectItem(json, "communication");
            if (!isJsonObject(json)) {
                handleParseConfigError();
            }

            subjson = cJSON_GetObjectItem(jitem, "pluginFileSuffixes");
            if (!isJsonObject(json)) {
                handleParseConfigError();
            }

            jitem = cJSON_GetObjectItem(subjson, "clientFileSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.clientFileSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("clientFileSuffix = %s\n", g_configData.clientFileSuffix);
            #endif


            jitem = cJSON_GetObjectItem(subjson, "serverFileSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.serverFileSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("serverFileSuffix = %s\n", g_configData.serverFileSuffix);
            #endif

            jitem = cJSON_GetObjectItem(json, "stateDirName");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.stateDirName = strdup(jitem->valuestring);
            #if DEBUG
                printf("stateDirName = %s\n", g_configData.stateDirName);
            #endif

            jitem = cJSON_GetObjectItem(json, "stateMapName");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.stateMapName = strdup(jitem->valuestring);
            #if DEBUG
                printf("stateMapName = %s\n", g_configData.stateMapName);
            #endif

            jitem = cJSON_GetObjectItem(json, "localaddr");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.localAddr = strdup(jitem->valuestring);
            #if DEBUG
                printf("localAddr = %s\n", g_configData.localAddr);
            #endif

            jitem = cJSON_GetObjectItem(json, "port_min");
            if (!isJsonNumber(jitem)) {
                handleParseConfigError();
            }

            g_configData.portMin = jitem->valueint;
            #if DEBUG
                printf("portMin = %d\n", g_configData.portMin);
            #endif

            jitem = cJSON_GetObjectItem(json, "port_max");
            if (!isJsonNumber(jitem)) {
                handleParseConfigError();
            }

            g_configData.portMax = jitem->valueint;
            #if DEBUG
                printf("portMax = %d\n", g_configData.portMax);
            #endif

            child = cJSON_GetObjectItem(json, "auth");
            if (!isJsonObject(json)) {
                handleParseConfigError();
            }

            jitem = cJSON_GetObjectItem(child, "keyDirSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.keyDirSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("keyDirSuffix = %s\n", g_configData.keyDirSuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "keyDirName");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.keyDirName = strdup(jitem->valuestring);
            #if DEBUG
                printf("keyDirName = %s\n", g_configData.keyDirName);
            #endif

            jitem = cJSON_GetObjectItem(child, "CAName");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.caName = strdup(jitem->valuestring);
            #if DEBUG
                printf("caName = %s\n", g_configData.caName);
            #endif

            jitem = cJSON_GetObjectItem(child, "privateKeyNameSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.privateKeyNameSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("privateKeyNameSuffix = %s\n", g_configData.privateKeyNameSuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "hostKeyNameSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.hostKeyNameSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("hostKeyNameSuffix = %s\n", g_configData.hostKeyNameSuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "clientKeyNameSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }
            g_configData.clientKeyNameSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("clientKeyNameSuffix = %s\n", g_configData.clientKeyNameSuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "SSLCertSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }
            g_configData.SSLCertSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("SSLCertSuffix = %s\n", g_configData.SSLCertSuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "SSLCertReqSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.SSLCertReqSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("SSLCertReqSuffix = %s\n", g_configData.SSLCertReqSuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "SSHPubKeySuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.SSHPubKeySuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("SSHPubKeySuffix = %s\n", g_configData.SSHPubKeySuffix);
            #endif

            jitem = cJSON_GetObjectItem(child, "defaultUser");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.defaultUser = strdup(jitem->valuestring);
            #if DEBUG
                printf("defaultUser = %s\n", g_configData.defaultUser);
            #endif

            jitem = cJSON_GetObjectItem(child, "SSHCertSuffix");
            if (!isJsonString(jitem)) {
                handleParseConfigError();
            }

            g_configData.SSHCertSuffix = strdup(jitem->valuestring);
            #if DEBUG
                printf("SSHCertSuffix = %s\n", g_configData.SSHCertSuffix);
            #endif


endParseConfig:
            cJSON_Delete(json);
        }

        free(buffer);
        fclose(fp); // close the file pointer
    }

    if(status) {
        concatUserDefinedConfigurations();
        loadLocalState();
        loadGlobalState(getGlobalStateLoc());
    }

    return status;
}


/*
* Helper snippet to print JSON parse errors.
*/
#define handleParseInterfacesError() \
{\
    status = false;\
    fprintf(stderr,"invalid JSON format for %s file\n", inf_file);\
    goto endParseInterfaces;\
}

/** Parses plugin interfaces. Loads the function names present in the corresponding plugin defined in the service specification JSON
* @param[in] inf_file file path for the plugin interface
* @return returns true upon successful parsing and false otherwise
*/
bool parsePluginInterfaces(char *inf_file) {
    char *out;
    int numentries = 0, i = 0;
    cJSON *json = NULL, *jitem, *child;
    bool status = true;
    FILE *fp = fopen(inf_file, "rb");

    if (fp == NULL) {
        fprintf(stderr,"Error can't open file %s\n", inf_file);
        status = false;
    } else {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        if (buffer != NULL) {
            fread(buffer, 1, size, fp);
            // parse the file
            json = cJSON_Parse(buffer);
        }
        if (json == NULL || !json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
            status = false;
        } else {
            #if DEBUG
                out = cJSON_Print(json, 2);
                printf("%s\n", out);
                free(out);
                out = NULL;
            #endif

            if (!isJsonObject(json)) {
                handleParseInterfacesError();
            }

            jitem = cJSON_GetObjectItem(json, "functions");
            if (!isJsonArray(jitem)) {
                handleParseInterfacesError();
            }

            child = jitem->child;
            /* How many entries in the array? */
            while (child && numentries++ < MAX_PROPERTIES) {
                child = child->next;
            }
            if (!numentries) {
                handleParseInterfacesError();
            }

            g_funcEntries = numentries;
            g_funcSignatures = (char **)malloc(numentries*sizeof(char*));
            if (!g_funcSignatures) {
                handleParseInterfacesError();
            }

            memset(g_funcSignatures,0,numentries*sizeof(char*));
            child = jitem->child;
            while (child) {
                if (!isJsonString(child)) {
                    handleParseInterfacesError();
                }

                g_funcSignatures[i] = strdup(child->valuestring);
                child = child->next;
                #if DEBUG
                    printf("g_funcSignature = %s\n", g_funcSignatures[i]);
                #endif
                i++;
            }

endParseInterfaces:
            cJSON_Delete(json);
        }

        free(buffer);
        fclose(fp); // close the file pointer
    }

    return status;
}

/** Frees all the memory allocated for global data
*/
void freeFuncSignatures() {
    int i;

    for(i = 0; i < g_funcEntries; i++) {
        free(g_funcSignatures[i]);
    }

    if (g_funcSignatures) {
        free(g_funcSignatures);
        g_funcSignatures = NULL;
        g_funcEntries = 0;
    }
}

/** Cleanup by freeing globals and client handler.
 * @param[in] commHandle communication handle
 */
void cleanUp(CommHandle **commHandle) {
    freeFuncSignatures();
    if(gCrypto && gCrypto->tunnelproc > 0) {
        destroySecureTunnel();
    }
    if (commHandle && *commHandle) {
        if ((*commHandle)->handle) {
            dlclose((*commHandle)->handle);
        }
        free(*commHandle);
        *commHandle = NULL;
    }
}

/** Cleanup by freeing service
 * @param[in] srvSpec service query or specification
 */
void cleanUpService(ServiceSpec **servSpec, CommHandle **commHandle) {
    int i;
    freeMDNSGlobals();

//    cleanUp(commHandle);

    if (servSpec && *servSpec) {
        ServiceSpec *srvSpec = *servSpec;
        if(srvSpec->service_name) {
            free(srvSpec->service_name);
            srvSpec->service_name = NULL;
        }
        if(srvSpec->type.name) {
            free(srvSpec->type.name);
            srvSpec->type.name = NULL;
        }
        if(srvSpec->type.protocol) {
            free(srvSpec->type.protocol);
            srvSpec->type.protocol = NULL;
        }
        if(srvSpec->address) {
            free(srvSpec->address);
            srvSpec->address = NULL;
        }
        if(srvSpec->comm_params) {
            for(i = 0; i < srvSpec->commParamsCount; i++) {
                if(srvSpec->comm_params[i]) {
                    if(srvSpec->comm_params[i]->key) {
                        free(srvSpec->comm_params[i]->key);
                    }
                    if(srvSpec->comm_params[i]->value) {
                        free(srvSpec->comm_params[i]->value);
                    }

                    free(srvSpec->comm_params[i]);
                    srvSpec->comm_params[i] = NULL;
                }
            }
            free(srvSpec->comm_params);
            srvSpec->comm_params = NULL;
        }
        if(srvSpec->properties) {
            for(i = 0; i < srvSpec->numProperties; i++) {
                if(srvSpec->properties[i]) {
                    if(srvSpec->properties[i]->key) {
                        free(srvSpec->properties[i]->key);
                        srvSpec->properties[i]->key = NULL;
                    }
                    if(srvSpec->properties[i]->value) {
                        free(srvSpec->properties[i]->value);
                        srvSpec->properties[i]->value = NULL;
                    }

                    free(srvSpec->properties[i]);
                    srvSpec->properties[i] = NULL;
                }
            }

            free(srvSpec->properties);
            srvSpec->properties = NULL;
        }
        if(srvSpec->advertise.locally) {
            free(srvSpec->advertise.locally);
            srvSpec->advertise.locally = NULL;
        }
        if(srvSpec->advertise.cloud) {
            free(srvSpec->advertise.cloud);
            srvSpec->advertise.cloud = NULL;
        }
        free(srvSpec);
        *servSpec = NULL;
    }
}

/** Initializes Communication handler. Loads the shared library and
* initializes Service communication handler with the specified functions defined in the plugin interface.
* @param[out] commHandle Service communication handler
* @return returns true upon successful initializing and false otherwise
*/
bool loadCommInterfaces(CommHandle *commHandle) {
    void *handle = commHandle->handle;
    int i;

    commHandle->interfacesCount = g_funcEntries;
    commHandle->interfaces = (Interfaces **)malloc(sizeof(Interfaces *) * g_funcEntries);
    if (commHandle->interfaces != NULL) {
        dlerror();  /* Clear any existing error */
        for(i = 0; i < g_funcEntries; i ++) {
            commHandle->interfaces[i] = (Interfaces *)malloc(sizeof(Interfaces));
            if (commHandle->interfaces[i] != NULL) {
                commHandle->interfaces[i]->iname = g_funcSignatures[i]; // copy the function name
                commHandle->interfaces[i]->iptr = dlsym(handle, g_funcSignatures[i]); // copy the function address - void pointer
                if (!checkDLError()) {
                    while(i >= 0) { // freeing the dynamic memory
                        free(commHandle->interfaces[i]);
                        commHandle->interfaces[i] = NULL;
                        i--;
                    }
                    free(commHandle->interfaces);
                    commHandle->interfaces = NULL;
                    return false;
                }
            }
        }
    }
    dlerror();  // Clear any existing error
    // this is a special function to initialize the plugin;
    // this function call takes Service specification as a parameter
    commHandle->init = (int (*)(void *)) dlsym(handle, "init");
    if (!checkDLError()) {
        commHandle->init = NULL;
    }

    return true;
}

/** Service Communication plugin loader. Loads the shared library and
* read the interface details.
* @param[in] plugin_path path to plugin
* @return returns service handle upon successful and NULL otherwise
*/
CommHandle *loadCommPlugin(char *plugin_path) {
    char *ptr;
    void *handle;
    CommHandle *commHandle = (CommHandle *)malloc(sizeof(CommHandle));
    if (commHandle == NULL) {
        fprintf(stderr,"Can't alloc memory for commHandle\n");
        return NULL;
    } else {
        commHandle->handle = NULL;

        handle = dlopen(plugin_path, RTLD_LAZY);
        if (!handle) {
            char *errmsg = dlerror();
            if (errmsg != NULL) {
                fprintf(stderr, "DL open error %s\n", errmsg);
            }
            commHandle->handle = NULL;
            free(commHandle); // free the dynamic memory
            return NULL;
        } else {
            dlerror();  /* Clear any existing error */
            commHandle->interface = (char **) dlsym(handle, "interface");
            if (!checkDLError()) {
                free(commHandle); // free the dynamic memory
                return NULL;
            }

            dlerror();  /* Clear any existing error */
            commHandle->provides_secure_comm = (bool *) dlsym(handle, "provides_secure_comm");
            if (!checkDLError()) {
                free(commHandle); // free the dynamic memory
                return NULL;
            }

            commHandle->handle = handle;
        }

        return commHandle;
    }
}

/** Initializes Client object. Creates the client object and calls its init method for further initialization
* @param[in] servQuery the service query object
* @return returns client handle upon successful and NULL otherwise
*/
CommHandle *createClient(ServiceQuery *servQuery) {
    CommHandle *commHandle = NULL;
    char cwd_temp[1024];
    char *mustsecure = NULL;

    // Parse configuration file
    strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
    strcat(cwd_temp, LIB_CONFIG_FILENAME);
    if (!parseConfigFile(cwd_temp)) {
        freeFuncSignatures();
    }

    // load the plugin
    // Considers the last path and loads the plugin interface
    char *substrStart = g_configData.pluginDir;
    char *substrEnd, *folderPath = NULL;
    do {
        if (*substrStart == ':') {
            substrStart++;
        }
        if (folderPath != NULL) { // Incase of iteration, freeing the previously allocated dynamic memory
            free(folderPath);
            folderPath = NULL;
        }
        #if DEBUG
            printf("\nsubstrStart path %s\n",substrStart);
        #endif
        substrEnd = strstr(substrStart, ":");
        if (substrEnd == NULL) {
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }
        #if DEBUG
            printf("\nfolderPath %s\n",folderPath);
        #endif
        strcpy(cwd_temp, ""); // set empty string
        // Parse plugin file
        if (folderPath != NULL && *folderPath != '/') { // if path is not absolute path; then consider plugins directory
            strcpy(cwd_temp, LIB_PLUGINS_DIRECTORY);
        }

        strcat(cwd_temp, "lib");
        strcat(cwd_temp, servQuery->type.name);
        strcat(cwd_temp, "-client.so");
        #if DEBUG
            printf("\nplugin name %s\n",cwd_temp);
        #endif
        if (fileExists(cwd_temp)) {
            commHandle = loadCommPlugin(cwd_temp);
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    if (!commHandle) {
        fprintf(stderr, "Plugin library \"lib%s-client.so\" not found\n", servQuery->type.name);
        cleanUp(&commHandle);
        free(folderPath);
        return NULL;
    }

    // Considers the last path and loads the plugin interface
    substrStart = g_configData.pluginInterfaceDir;
    do {

        if (*substrStart == ':') {
            substrStart++;
        }
        if (folderPath != NULL) { // Incase of iteration, freeing the previously allocated dynamic memory
            free(folderPath);
            folderPath = NULL;
        }
        substrEnd = strstr(substrStart, ":");
        if (substrEnd == NULL) {
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }

        strcpy(cwd_temp, ""); // set empty string
        // Parse plugin interface file
        if (folderPath != NULL && *folderPath != '/') { // if path is not absolute path; then consider plugins directory
            strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
        }

        if (folderPath != NULL) {
            strcat(cwd_temp, folderPath);
        }
        strcat(cwd_temp, "/");
        strcat(cwd_temp, *(commHandle->interface));
        strcat(cwd_temp, ".json");
        #if DEBUG
            printf("\ninterface file name %s\n",cwd_temp);
        #endif
        if (fileExists(cwd_temp)) {
            if (!parsePluginInterfaces(cwd_temp)) {
                freeFuncSignatures();
            }
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    free(folderPath); // free the dynamic memory
    if (loadCommInterfaces(commHandle) == false) {
        cleanUp(&commHandle);
        return NULL;
    }

    mustsecure = servQuery->type_params.mustsecure ? servQuery->type_params.mustsecure : getSpecPropertyValue(servQuery, "__mustsecure");
    if(mustsecure) {
        gCrypto = crypto_init();
        if(gCrypto && *commHandle->provides_secure_comm == false) {
            // no credentials setup and plugin does not provide own security mechanism
            fprintf(stderr, "Cannot connect securely because credentials are not setup (a secure\n"
                                    "communication channel was requested either by the service or the client).\n"
                                    "Run iotkit-comm setupAuthentication to create and configure credentials.");
        }

        if(getSpecPropertyValue(servQuery, "__mustsecure") == NULL && \
            getSpecPropertyValue(servQuery, "__cansecure") == NULL && \
            getSpecPropertyValue(servQuery, "__user") == NULL ) {
                // cannot create secure channel because server is not configured for it
                fprintf(stderr, "Cannot connect securely because the server does not support secure communications.");
        }

        if(gCrypto && *commHandle->provides_secure_comm == false) {
            // create secure tunnel
            int localport = 0;
            char *localaddr = NULL;
            bool isTunnelCreated = false;
            printf("Setting up secure communication chanel...\n");

            isTunnelCreated = createSecureTunnel(servQuery, &localport, &localaddr);
            if(isTunnelCreated == true) {
                servQuery->address = localaddr;
                servQuery->port = localport;
                printf("Secure tunnel setup at %s:%d\n", servQuery->address, servQuery->port);
            }
        }
    }

    if (commHandle->init) {
        commHandle->init(servQuery, gCrypto);
    }

    return commHandle;
}


/** Checks for file existence.
* @param[in] absPath absolute path of a file
* @return returns true if the file exists and false otherwise
*/
bool fileExists(char *absPath) {
    FILE *fp;
    if (fp = fopen(absPath, "r")) {
        fclose(fp);
        return true;
    }

    return false;
}

CommHandle *loadService(ServiceSpec *specification) {
    CommHandle *commHandle = NULL;
    char cwd_temp[1024];

    // Parse configuration file
    strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
    strcat(cwd_temp, LIB_CONFIG_FILENAME);
    if (!parseConfigFile(cwd_temp)) {
        freeFuncSignatures();
    }

    // load the plugin
    // Considers the last path and loads the plugin interface
    char *substrStart = g_configData.pluginDir;
    char *substrEnd, *folderPath = NULL;
    do {
        if (*substrStart == ':') {
            substrStart++;
        }
        if (folderPath != NULL) { // Incase of iteration, freeing the previously allocated dynamic memory
            free(folderPath);
            folderPath = NULL;
        }
        substrEnd = strstr(substrStart, ":");
        if (substrEnd == NULL) {
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }

        strcpy(cwd_temp, ""); // set empty string

        // Parse plugin file
        if (folderPath != NULL && *folderPath != '/') { // if path is not absolute path; then consider plugins directory
            strcpy(cwd_temp, LIB_PLUGINS_DIRECTORY);
        }

        strcat(cwd_temp, "lib");
        strcat(cwd_temp, specification->type.name);
        strcat(cwd_temp, "-service.so");

        if (fileExists(cwd_temp)) {
            commHandle = loadCommPlugin(cwd_temp);
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    if (!commHandle) {
        fprintf(stderr, "Plugin library \"lib%s-service.so\" not found\n", specification->type.name);
        cleanUp(&commHandle);
        free(folderPath);
        return NULL;
    }

    // Considers the last path and loads the plugin interface
    substrStart = g_configData.pluginInterfaceDir;
    do {
        if (*substrStart == ':') {
            substrStart++;
        }
        if (folderPath != NULL) { // Incase of iteration, freeing the previously allocated dynamic memory
            free(folderPath);
            folderPath = NULL;
        }
        substrEnd = strstr(substrStart, ":");
        if (substrEnd == NULL) {
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }

        strcpy(cwd_temp, ""); // set empty string

        // Parse plugin interface file
        if (folderPath != NULL && *folderPath != '/') { // if path is not absolute path; then consider plugins directory
            strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
        }
        if (folderPath != NULL) {
            strcat(cwd_temp, folderPath);
        }
        strcat(cwd_temp, "/");
        strcat(cwd_temp, *(commHandle->interface));
        strcat(cwd_temp, ".json");

        if (fileExists(cwd_temp)) {
            if (!parsePluginInterfaces(cwd_temp)) {
                freeFuncSignatures();
            }
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    free(folderPath); // free the dynamic memory
    if (loadCommInterfaces(commHandle) == false) {
        cleanUp(&commHandle);
        return NULL;
    }

    gCrypto = crypto_init();
    if(specification->type_params.mustsecure) {
        // service wants clients to only connect securely
        if(gCrypto == NULL && *(commHandle->provides_secure_comm) == false) {
            fprintf(stderr, "Service expects clients to connect securely but credentials are not setup.\n"
                                  "Run iotkit-comm setupAuthentication to create and configure credentials.\n");
        }

        addSpecProperty(specification, "__mustsecure", "true");
    }

    if(gCrypto && *(commHandle->provides_secure_comm) == false) {
        // plugin does not provide its own security mechanisms.
        // so, allow clients to connect securely through SSH tunnels if they wish
        //   - advertise the user account (on this machine) that will be used to create the secure tunnels.
        addSpecProperty(specification, "__user", gCrypto->user);
    } else if(*(commHandle->provides_secure_comm)) {
        // plugin provides own security mechanism.
        char *spec_isSecure = getSpecPropertyValue(specification, "__mustsecure");
        if(spec_isSecure && strcmp(spec_isSecure, "false") == 0) {
            addSpecProperty(specification, "__cansecure", "true");
        }
    }

    return commHandle;
}

/** Initializes Service object. Creates the service object and calls its init method for further initialization.
* @param[in] specification service specification
* @return returns service handle upon successful and NULL otherwise
*/
CommHandle *createService(CommHandle *commHandle, ServiceSpec *specification) {

    if(!commHandle) {
        // load the plugin if the object is not available
        commHandle = loadService(specification);
    }

    if(specification->type_params.mustsecure) {
        gCrypto = crypto_init();
    }

    if (commHandle && commHandle->init) {
        commHandle->init(specification, gCrypto);
    }

    return commHandle;
}

void* commInterfacesLookup(CommHandle *commHandle, char *funcname) {
    int i;

    for(i = 0; i < commHandle->interfacesCount; i ++) {
        if (strcmp(commHandle->interfaces[i]->iname, funcname) == 0) {
            return &(commHandle->interfaces[i]->iptr);
        }
    }

    return NULL;
}

char *getSpecPropertyValue(ServiceSpec *specification, char *key) {
    if(specification->numProperties > 0 && specification->properties) {
        int i = 0;
        for(i = 0; i < specification->numProperties; i ++) {
            Property *property = specification->properties[i];

            if(strcmp(property->key, key) == 0) {
                return strdup(property->value);
            }
        }
    }

    return NULL;
}

void addSpecProperty(ServiceSpec *specification, char *key, char *value) {
    Property *property = (Property *)malloc(sizeof(Property));
    property->key = strdup(key);
    property->value = strdup(value);
    specification->numProperties ++;
    specification->properties = (Property **)realloc(specification->properties, sizeof(Property *) * specification->numProperties);
    specification->properties[specification->numProperties - 1] = property;
}

Crypto *crypto_init() {
    char *home;
    Crypto *lCrypto = NULL;
    lCrypto = (Crypto *)malloc(sizeof(Crypto));

    if(!lCrypto) {
        fprintf(stderr, "Could not allocate memory for crypto");
        return NULL;
    }
    if(!g_configData.localState || !g_configData.globalState) {
        fprintf(stderr, "No credentials found. Please create credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    if(strcmp(g_configData.localState->host, g_configData.globalState->host) != 0 ) {
        fprintf(stderr, "Credentials do not seem to have been setup correctly for this host. Please rerun "
               "iotkit-comm setupAuthentication");
        return NULL;
    }

    lCrypto->user = g_configData.localState->user;
    lCrypto->host = g_configData.localState->host;
    lCrypto->mosquittoSecurePort = g_configData.globalState->mosquittoSecurePort;

    strcpy(lCrypto->cacert, getGlobalStateLoc());
    if (*(lCrypto->cacert + (strlen(lCrypto->cacert) - 1)) != '/') {
        strcat(lCrypto->cacert, "/");
    }
    strcat(lCrypto->cacert, g_configData.stateDirName);
    strcat(lCrypto->cacert, "/");
    strcat(lCrypto->cacert, g_configData.caName);
    strcat(lCrypto->cacert, g_configData.keyDirSuffix);
    strcat(lCrypto->cacert, "/");
    strcat(lCrypto->cacert, g_configData.caName);
    strcat(lCrypto->cacert, g_configData.SSLCertSuffix);

    if(!fileExists(lCrypto->cacert)) {
        fprintf(stderr, "No CA certificate found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    home = getenv("HOME");
    if (home == NULL || strlen(home) == 0) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
    }

    strcpy(lCrypto->hostpubkey, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->hostpubkey, "/");
    }

    strcat(lCrypto->hostpubkey, ".");
    strcat(lCrypto->hostpubkey, g_configData.stateDirName);
    strcat(lCrypto->hostpubkey, "/");
    strcat(lCrypto->hostpubkey, lCrypto->host);
    strcat(lCrypto->hostpubkey, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostpubkey, g_configData.keyDirSuffix);
    strcat(lCrypto->hostpubkey, "/");
    strcat(lCrypto->hostpubkey, lCrypto->host);
    strcat(lCrypto->hostpubkey, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostpubkey, g_configData.privateKeyNameSuffix);
    strcat(lCrypto->hostpubkey, g_configData.SSHPubKeySuffix);

    if(!fileExists(lCrypto->hostpubkey)) {
        fprintf(stderr, "No host public key found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->hostkey, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->hostkey, "/");
    }

    strcat(lCrypto->hostkey, ".");
    strcat(lCrypto->hostkey, g_configData.stateDirName);
    strcat(lCrypto->hostkey, "/");
    strcat(lCrypto->hostkey, lCrypto->host);
    strcat(lCrypto->hostkey, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostkey, g_configData.keyDirSuffix);
    strcat(lCrypto->hostkey, "/");
    strcat(lCrypto->hostkey, lCrypto->host);
    strcat(lCrypto->hostkey, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostkey, g_configData.privateKeyNameSuffix);

    if(!fileExists(lCrypto->hostkey)) {
        fprintf(stderr, "No host private key found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->hostsshcert, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->hostsshcert, "/");
    }

    strcat(lCrypto->hostsshcert, ".");
    strcat(lCrypto->hostsshcert, g_configData.stateDirName);
    strcat(lCrypto->hostsshcert, "/");
    strcat(lCrypto->hostsshcert, lCrypto->host);
    strcat(lCrypto->hostsshcert, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostsshcert, g_configData.keyDirSuffix);
    strcat(lCrypto->hostsshcert, "/");
    strcat(lCrypto->hostsshcert, lCrypto->host);
    strcat(lCrypto->hostsshcert, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostsshcert, g_configData.privateKeyNameSuffix);
    strcat(lCrypto->hostsshcert, g_configData.SSHCertSuffix);
    strcat(lCrypto->hostsshcert, g_configData.SSHPubKeySuffix);

    if(!fileExists(lCrypto->hostsshcert)) {
        fprintf(stderr, "No host SSH certificate found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->hostsslcert, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->hostsslcert, "/");
    }
    strcat(lCrypto->hostsslcert, ".");
    strcat(lCrypto->hostsslcert, g_configData.stateDirName);
    strcat(lCrypto->hostsslcert, "/");
    strcat(lCrypto->hostsslcert, lCrypto->host);
    strcat(lCrypto->hostsslcert, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostsslcert, g_configData.keyDirSuffix);
    strcat(lCrypto->hostsslcert, "/");
    strcat(lCrypto->hostsslcert, lCrypto->host);
    strcat(lCrypto->hostsslcert, g_configData.hostKeyNameSuffix);
    strcat(lCrypto->hostsslcert, g_configData.privateKeyNameSuffix);
    strcat(lCrypto->hostsslcert, g_configData.SSLCertSuffix);

    if(!fileExists(lCrypto->hostsslcert)) {
        fprintf(stderr, "No host SSL certificate found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->userkey, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->userkey, "/");
    }
    strcat(lCrypto->userkey, ".");
    strcat(lCrypto->userkey, g_configData.stateDirName);
    strcat(lCrypto->userkey, "/");
    strcat(lCrypto->userkey, lCrypto->host);
    strcat(lCrypto->userkey, "_");
    strcat(lCrypto->userkey, lCrypto->user);
    strcat(lCrypto->userkey, g_configData.keyDirSuffix);
    strcat(lCrypto->userkey, "/");
    strcat(lCrypto->userkey, lCrypto->host);
    strcat(lCrypto->userkey, "_");
    strcat(lCrypto->userkey, lCrypto->user);
    strcat(lCrypto->userkey, g_configData.privateKeyNameSuffix);

    if(!fileExists(lCrypto->userkey)) {
        fprintf(stderr, "No user private key found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->userpubkey, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->userpubkey, "/");
    }
    strcat(lCrypto->userpubkey, ".");
    strcat(lCrypto->userpubkey, g_configData.stateDirName);
    strcat(lCrypto->userpubkey, "/");
    strcat(lCrypto->userpubkey, lCrypto->host);
    strcat(lCrypto->userpubkey, "_");
    strcat(lCrypto->userpubkey, lCrypto->user);
    strcat(lCrypto->userpubkey, g_configData.keyDirSuffix);
    strcat(lCrypto->userpubkey, "/");
    strcat(lCrypto->userpubkey, lCrypto->host);
    strcat(lCrypto->userpubkey, "_");
    strcat(lCrypto->userpubkey, lCrypto->user);
    strcat(lCrypto->userpubkey, g_configData.privateKeyNameSuffix);
    strcat(lCrypto->userpubkey, g_configData.SSHPubKeySuffix);

    if(!fileExists(lCrypto->userpubkey)) {
        fprintf(stderr, "No user public key found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->usersshcert, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->usersshcert, "/");
    }
    strcat(lCrypto->usersshcert, ".");
    strcat(lCrypto->usersshcert, g_configData.stateDirName);
    strcat(lCrypto->usersshcert, "/");
    strcat(lCrypto->usersshcert, lCrypto->host);
    strcat(lCrypto->usersshcert, "_");
    strcat(lCrypto->usersshcert, lCrypto->user);
    strcat(lCrypto->usersshcert, g_configData.keyDirSuffix);
    strcat(lCrypto->usersshcert, "/");
    strcat(lCrypto->usersshcert, lCrypto->host);
    strcat(lCrypto->usersshcert, "_");
    strcat(lCrypto->usersshcert, lCrypto->user);
    strcat(lCrypto->usersshcert, g_configData.privateKeyNameSuffix);
    strcat(lCrypto->usersshcert, g_configData.SSHCertSuffix);
    strcat(lCrypto->usersshcert, g_configData.SSHPubKeySuffix);

    if(!fileExists(lCrypto->usersshcert)) {
        fprintf(stderr, "No user SSH certificate found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }

    strcpy(lCrypto->usersslcert, home);
    if (*(home + (strlen(home) - 1)) != '/') {
        strcat(lCrypto->usersslcert, "/");
    }
    strcat(lCrypto->usersslcert, ".");
    strcat(lCrypto->usersslcert, g_configData.stateDirName);
    strcat(lCrypto->usersslcert, "/");
    strcat(lCrypto->usersslcert, lCrypto->host);
    strcat(lCrypto->usersslcert, "_");
    strcat(lCrypto->usersslcert, lCrypto->user);
    strcat(lCrypto->usersslcert, g_configData.keyDirSuffix);
    strcat(lCrypto->usersslcert, "/");
    strcat(lCrypto->usersslcert, lCrypto->host);
    strcat(lCrypto->usersslcert, "_");
    strcat(lCrypto->usersslcert, lCrypto->user);
    strcat(lCrypto->usersslcert, g_configData.privateKeyNameSuffix);
    strcat(lCrypto->usersslcert, g_configData.SSLCertSuffix);

    if(!fileExists(lCrypto->usersslcert)) {
        fprintf(stderr, "No user SSL certificate found. Please recreate credentials using iotkit-comm setupAuthentication");
        return NULL;
    }


    lCrypto->tunnelproc = -1;
    lCrypto->destroyingTunnel = false;
    lCrypto->portInUse = 0;
    lCrypto->successRE = SSH_TUNNEL_CMD_SUCCESS_RE_STRING;
    lCrypto->portInUseRE = SSH_TUNNEL_ADDRESS_IN_USE_RE_STRING;


    return lCrypto;
}

int destroySecureTunnel() {
    if(gCrypto->tunnelproc > 0) {
        int internal_stat;
        kill(gCrypto->tunnelproc, SIGKILL);
        waitpid(gCrypto->tunnelproc, &internal_stat, 0);
        gCrypto->tunnelproc = -1;
        return WEXITSTATUS(internal_stat);
    }
}

int getRandomPort(int min_port, int max_port) {
    return (rand() % (max_port - min_port)) + min_port;
}

char **getCreateSecureTunnelArgs(char *remoteHost, int localPort, int remotePort, char *remoteUser) {
    char lPort[256];
    int argumentCount = 9, i;
    char **arguments;

    arguments = (char **)malloc(sizeof(char *) * argumentCount);
    for(i = 0; i < argumentCount; i ++) {
        arguments[i] = (char *)malloc(sizeof(char) * 256);
    }

    strcpy(arguments[0], "/usr/bin/ssh");
    strcpy(arguments[1], "-v");

    strcpy(arguments[2], remoteUser);
    strcat(arguments[2], "@");
    strcat(arguments[2], remoteHost);

    strcpy(arguments[3], "-L");

    sprintf(lPort, "%d", localPort);
    strcpy(arguments[4], lPort);
    strcat(arguments[4], ":");
    strcat(arguments[4], remoteHost);
    strcat(arguments[4], ":");
    sprintf(lPort, "%d", remotePort);
    strcat(arguments[4], lPort);

    strcpy(arguments[5], " -o ");

    strcpy(arguments[6], "ExitOnForwardFailure=yes");
    strcpy(arguments[7], " -N");
    *(arguments + 8) = NULL;

    return arguments;
}

pid_t popen2(const char **arguments, int *infp, int *outfp)
{
    int p_stdin[2], p_stdout[2], p_stderr[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0 || pipe(p_stderr) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        dup2(p_stdin[READ], STDIN_FILENO);
        dup2(p_stdout[WRITE], STDOUT_FILENO);
        dup2(p_stderr[WRITE], STDERR_FILENO);

        //close unused descriptors on child process.
        close(p_stdin[READ]);
        close(p_stdin[WRITE]);
        close(p_stdout[READ]);
        close(p_stdout[WRITE]);
        close(p_stderr[READ]);
        close(p_stderr[WRITE]);

        execl(arguments[0], arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7], arguments[8]);
        perror("execl");
        exit(1);
    }

    // close unused descriptors on parent process.
    close(p_stdin[READ]);
    close(p_stdout[WRITE]);
    close(p_stderr[WRITE]);

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL) {
        close(p_stdout[READ]);
        close(p_stderr[READ]);
    } else {
        close(p_stdout[READ]);
        *outfp = p_stderr[READ];
        // *outfp = p_stdout[READ];
    }

    return pid;
}

bool startTunnel(ServiceSpec *specification, int *localport, char **localaddr) {
    regex_t regex_success;
    regex_t regex_portInUse;
    FILE *stream = NULL;
    char command[1024];
    char buffer[512];
    char processData[12288]; // 12 KB
    int res;
    char msgbuf[100];
    int numOfRetries = 0;

    /* Compile regular expression */
    res = regcomp(&regex_success, gCrypto->successRE, REG_EXTENDED);
    if(res ) {
        fprintf(stderr, "Could not compile regex for \'%s\'\n", gCrypto->successRE);
    }

    res = regcomp(&regex_portInUse, gCrypto->portInUseRE, REG_EXTENDED);
    if(res ) {
        fprintf(stderr, "Could not compile regex for \'%s\'\n", gCrypto->portInUseRE);
    }

    srand(time(NULL)); // do the seeding for pseudo random number generator

    do {
        int localPort = getRandomPort(g_configData.portMin, g_configData.portMax);
        char *user = getSpecPropertyValue(specification, "__user");
        char **args = getCreateSecureTunnelArgs(specification->address, localPort, specification->port, user);
        int infp = 0, outfp = 0;
        int count;

        gCrypto->portInUse = 0;
        gCrypto->tunnelproc = popen2(args, &infp, &outfp);
        if(outfp) {
            strcpy(processData, ""); // Nullify processData

            while((count = read(outfp, buffer, sizeof(char) * 512)) > 0) {
                    buffer[count] = '\0'; // NULL termination
                    strcat(processData, buffer);
                    #if DEBUG
                        printf("SSH verbose:: %s\n", buffer);
                    #endif

                    /* Execute regular expression */
                    res = regexec(&regex_success, processData, 0, NULL, 0);
                    if( !res ) {
                        *localport = localPort;
                        *localaddr = strdup(g_configData.localAddr);
                        close(outfp);
                        /* Free compiled regular expression if you want to use the regex_t again */
                        regfree(&regex_success);
                        regfree(&regex_portInUse);
                        return true;
                    }

                    /* Execute regular expression */
                    res = regexec(&regex_portInUse, processData, 0, NULL, 0);
                    if( !res ) {
                        // port already in use
                        gCrypto->portInUse = localPort;
                        break;
                    }
            }

            close(outfp);
        }

        if(gCrypto->portInUse != 0) {
            fprintf(stderr, "Could not create tunnel at chosen port \'%d\', trying again with a new port...", gCrypto->portInUse);
            gCrypto->tunnelproc = -1;
            gCrypto->portInUse = 0;
        }

        numOfRetries ++;
    } while(gCrypto->portInUse == 0 && numOfRetries < 3); // try 3 times and quit to avoid infinite loop when no free port is available

    /* Free compiled regular expression if you want to use the regex_t again */
    regfree(&regex_success);
    regfree(&regex_portInUse);

    return false;
}

bool createSecureTunnel(ServiceSpec *specification, int *localport, char **localaddr) {
    char *user = getSpecPropertyValue(specification, "__user");
    if(gCrypto->tunnelproc > 0) {
        fprintf(stderr, "Warning: A secure tunnel already exists. To create a new one, use a new instance");

        *localport = 0;
        *localaddr = NULL;

        return false;
    }

    if (!specification->properties || !user || !specification->port || !specification->address) {
        fprintf(stderr, "Could not create secure tunnel to service:\n");
        return false;
    }

    return startTunnel(specification, localport, localaddr);
}

#if DEBUG
int main(int argc, char *argv[]) {
    ServiceSpec *specification = (ServiceSpec *)parseServiceSpec("../../examples/serviceSpecs/temperatureServiceMQTT.json");
    CommHandle *commHandle = createClient(specification);

    if (commHandle) {
        Context context;
        context.name = "topic";
        context.value = "temperature";
        commHandle->send("75 degrees", context);
        commHandle->unsubscribe("temperature");
        commHandle->subscribe("temperature");
    }
}
#endif
