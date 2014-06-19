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

/**
 * @file edisonapi.c
 * @brief Implementation of Edison Library
 *
 * Provides features to create client,service, to load communication interfaces as well as to load the Plugins.
 */

#include "edisonapi.h"

#include "cJSON.h"
#include "util.h"

#ifndef DEBUG
    #define DEBUG 0
#endif


/** Reports dynamic link library errors. Checks for any recent error happened while loading an shared library
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
    fprintf(stderr,"invalid JSON format for %s file\n", config_file);\
    goto endParseConfig;\
}

/** Considers user defined configuration. Loads and parses user defined configuration
* '.edison-config.json' present in user's home directory for 'plugin interface' and
* 'plugins' configuration properties and concats with edison configuration properties
 */
void concatUserDefinedConfigurations(){
    char *home, config_file[1024];

    home = getenv("HOME");
    if(home == NULL || strlen(home) == 0){
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
    }

    strcpy(config_file, home);
    if(*(home + (strlen(home) - 1)) != '/')
        strcat(config_file, "/");

    strcat(config_file, USER_CONFIG_FILENAME);

       char *out;
           cJSON *json, *jitem;
           FILE *fp = fopen(config_file, "rb");

           if (fp == NULL)
           {
       	    fprintf(stderr,"Warning: Optional user config file not found. Continuing...\n");
           }
           else
           {
       	fseek(fp, 0, SEEK_END);
       	long size = ftell(fp);
       	rewind(fp);

       	// read the file
       	char *buffer = (char *)malloc(size+1);
       	fread(buffer, 1, size, fp);

       	// parse the file
       	json = cJSON_Parse(buffer);
               if (!json)
       	{
                   fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
               }
       	else
       	{
                   #if DEBUG
                   out = cJSON_Print(json, 2);
                   fprintf(stderr,"%s\n", out);
                   free(out);
                   #endif

                   if (!isJsonObject(json)) handleParseConfigError();

                   jitem = cJSON_GetObjectItem(json, "pluginInterfaceDirPaths");
                   if (!isJsonString(jitem)) handleParseConfigError();

                    int pathSize = strlen(g_configData.pluginInterfaceDir) + strlen(jitem->valuestring) + 2;
                    char *pluginInterfaceDirPaths = (char *)malloc(sizeof(char) * pathSize);
                   strcpy(pluginInterfaceDirPaths, g_configData.pluginInterfaceDir);
                   strcat(pluginInterfaceDirPaths, ":");
                   strcat(pluginInterfaceDirPaths, jitem->valuestring);
                   g_configData.pluginInterfaceDir = pluginInterfaceDirPaths;
                   #if DEBUG
                   printf("pluginInterfaceDir = %s\n", g_configData.pluginInterfaceDir);
                   #endif

                   jitem = cJSON_GetObjectItem(json, "pluginDirPaths");
                   if (!isJsonString(jitem)) handleParseConfigError();

                    pathSize = strlen(g_configData.pluginDir) + strlen(jitem->valuestring) + 2;
                    char *pluginDirPaths = (char *)malloc(sizeof(char) * pathSize);
                   strcpy(pluginDirPaths, g_configData.pluginDir);
                   strcat(pluginDirPaths, ":");
                   strcat(pluginDirPaths, jitem->valuestring);
                   g_configData.pluginDir = pluginDirPaths;
                   #if DEBUG
                   printf("pluginDir = %s\n", g_configData.pluginDir);
                   #endif




       endParseConfig:
                   cJSON_Delete(json);
               }

               free(buffer);
           }
}

/** Parses edison configuration file
 * @param[in] config_file path to the configuration file
 */
bool parseConfigFile(char *config_file)
{
    char *out;
    cJSON *json, *jitem, *child, *subjson;
    bool status = true;
    FILE *fp = fopen(config_file, "rb");

    if (fp == NULL) 
    {
	fprintf(stderr,"Error can't open file %s\n", config_file);
	status = false;
    }
    else 
    {
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	// read the file
	char *buffer = (char *)malloc(size+1);
	fread(buffer, 1, size, fp);

	// parse the file
	json = cJSON_Parse(buffer);
        if (!json) 
	{
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
            status = false;
        } 
	else 
	{
            #if DEBUG
            out = cJSON_Print(json, 2);
            fprintf(stderr,"%s\n", out);
            free(out);
            #endif

            if (!isJsonObject(json)) handleParseConfigError();

            jitem = cJSON_GetObjectItem(json, "pluginInterfaceDir");
            if (!isJsonString(jitem)) handleParseConfigError();

            g_configData.pluginInterfaceDir = strdup(jitem->valuestring);
            #if DEBUG
            printf("pluginInterfaceDir = %s\n", g_configData.pluginInterfaceDir);
            #endif
                                                                
            jitem = cJSON_GetObjectItem(json, "pluginDir");
            if (!isJsonString(jitem)) handleParseConfigError();

            g_configData.pluginDir = strdup(jitem->valuestring);
            #if DEBUG
            printf("pluginDir = %s\n", g_configData.pluginDir);
            #endif


            jitem = cJSON_GetObjectItem(json, "communication");
            if (!isJsonObject(json)) handleParseConfigError();

            subjson = cJSON_GetObjectItem(jitem, "pluginFileSuffixes");
            if (!isJsonObject(json)) handleParseConfigError();



            jitem = cJSON_GetObjectItem(subjson, "clientFileSuffix");
            if (!isJsonString(jitem)) handleParseConfigError();

            g_configData.clientFileSuffix = strdup(jitem->valuestring);
            #if DEBUG
            printf("clientFileSuffix = %s\n", g_configData.clientFileSuffix);
            #endif


            jitem = cJSON_GetObjectItem(subjson, "serverFileSuffix");
            if (!isJsonString(jitem)) handleParseConfigError();

            g_configData.serverFileSuffix = strdup(jitem->valuestring);
            #if DEBUG
            printf("serverFileSuffix = %s\n", g_configData.serverFileSuffix);
            #endif


endParseConfig:
            cJSON_Delete(json);
        }

	// free buffers
        free(buffer);
    }

    concatUserDefinedConfigurations();

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

/** Parses plugin interfaces. Loads the function names present in the corresponding plugin defined in the service description JSON
 * @param[in] inf_file file path for the plugin interface
 * @return returns true upon successful parsing and false otherwise
 */
bool parsePluginInterfaces(char *inf_file) 
{
    char *out;
    int numentries=0, i=0;
    cJSON *json, *jitem, *child;
    bool status = true;
    FILE *fp = fopen(inf_file, "rb");

    if (fp == NULL) 
    {
        fprintf(stderr,"Error can't open file %s\n", inf_file);
        status = false;
    }
    else 
    {
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        rewind(fp);

        // read the file
        char *buffer = (char *)malloc(size+1);
        fread(buffer, 1, size, fp);

        // parse the file
        json = cJSON_Parse(buffer);
	if (!json) 
	{
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
            status = false;
        } 
	else 
	{
            #if DEBUG
            out = cJSON_Print(json, 2);
            printf("%s\n", out);
            free(out);
            #endif

            if (!isJsonObject(json)) handleParseInterfacesError();

            jitem = cJSON_GetObjectItem(json, "functions");
            if (!isJsonArray(jitem)) handleParseInterfacesError();

            child = jitem->child;
            /* How many entries in the array? */
            while (child) numentries++,child=child->next; 
            if (!numentries) handleParseInterfacesError();

            g_funcEntries = numentries;
            g_funcSignatures = (char **)malloc(numentries*sizeof(char*));
            if (!g_funcSignatures) handleParseInterfacesError();

            memset(g_funcSignatures,0,numentries*sizeof(char*));
            child=jitem->child;
            while (child)
            {
		        if (!isJsonString(child)) handleParseInterfacesError();

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

        // free buffers
        free(buffer);
    }

    return status;
}

/** Frees all the memory allocated for global data
 */
void freeGlobals()
{
    if (g_funcSignatures) {
	free(g_funcSignatures);
    }
}

/** clean up by freeing globals and client handler
 * @param[in] commHandle communication handle
 */
void cleanUp(CommHandle *commHandle)
{
    freeGlobals();
    if (commHandle) 
    {
	if (commHandle->handle) {
	    dlclose(commHandle->handle); 
	}
	free(commHandle); 
    }
}

/** Initializes Communication handler. Loads the shared library and
 * initializes Service communication handler with the specified functions defined in the plugin interface
 * @param[out] commHandle Service communication handler
 * @return returns true upon successful initializing and false otherwise
 */
bool loadCommInterfaces(CommHandle *commHandle){

    void *handle = commHandle->handle;
    int i;

    commHandle->interfacesCount = g_funcEntries;
    commHandle->interfaces = (Interfaces **)malloc(sizeof(Interfaces *) * g_funcEntries);

    dlerror();	/* Clear any existing error */
    for(i = 0; i < g_funcEntries; i ++){
        commHandle->interfaces[i] = (Interfaces *)malloc(sizeof(Interfaces));

        commHandle->interfaces[i]->iname = g_funcSignatures[i]; // copy the function name
        commHandle->interfaces[i]->iptr = dlsym(handle, g_funcSignatures[i]); // copy the function address - void pointer
        if (!checkDLError()) return false;
    }

    dlerror();	// Clear any existing error
    // this is a special function to initialize the plugin;
    // this function call takes Service Description as a parameter
	commHandle->init = (int (*)(void *)) dlsym(handle, "init");
	if (!checkDLError()) {
	    commHandle->init = NULL;
	}

	return true;
}

/** Service Communication plugin loader. Loads the shared library and
 * read the interface details
 * @param[in] plugin_path path to plugin
 * @return returns service handle upon successful and NULL otherwise
 */
CommHandle *loadCommPlugin(char *plugin_path)
{
    char *ptr;
    void *handle;
    CommHandle *commHandle = (CommHandle *)malloc(sizeof(CommHandle));
    if (commHandle == NULL)
    {
	fprintf(stderr,"Can't alloc memory for commHandle\n");
	return NULL;
    }
    else
    {
    commHandle->handle = NULL;

	handle = dlopen(plugin_path, RTLD_LAZY);
	if (!handle) 
	{
	    fprintf(stderr, "DL open error %s\n", dlerror());
	    commHandle->handle = NULL;
	    return NULL; 
	}
	else
	{

        dlerror();	/* Clear any existing error */
	    commHandle->interface = (char **) dlsym(handle, "interface");
	    if (!checkDLError()) return NULL;

	    commHandle->handle = handle;
	}

	return commHandle;
    }
}


/** Initializes Client object. Creates the client object and calls its init method for further initialization
 * @param[in] queryDesc query description
 * @return returns client handle upon successful and NULL otherwise
 */
CommHandle *createClient(ServiceQuery *queryDesc)
{
    CommHandle *commHandle = NULL;
    char cwd_temp[1024];

    // Parse configuration file
    strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
    strcat(cwd_temp, LIB_CONFIG_FILENAME);
    if (!parseConfigFile(cwd_temp)) freeGlobals();

    // load the plugin
    // Considers the last path and loads the plugin interface
    char *substrStart = g_configData.pluginDir;
    char *substrEnd, *folderPath;
    do{
        if(*substrStart == ':') substrStart++;
        #if DEBUG
            printf("\nsubstrStart path %s\n",substrStart);
        #endif
        substrEnd = strstr(substrStart, ":");
        if(substrEnd == NULL){
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }
        #if DEBUG
            printf("\nfolderPath %s\n",folderPath);
        #endif
        strcpy(cwd_temp, ""); // set empty string
        // Parse plugin file
        if(*folderPath != '/'){ // if path is not absolute path; then consider edisonlib directory
            strcpy(cwd_temp, LIB_PLUGINS_DIRECTORY);
        }

//        strcat(cwd_temp, folderPath);
        strcat(cwd_temp, "lib");
        strcat(cwd_temp, queryDesc->type.name);
        strcat(cwd_temp, "-client.so");
        #if DEBUG
            printf("\nplugin name %s\n",cwd_temp);
        #endif
        if(fileExists(cwd_temp)){
            commHandle = loadCommPlugin(cwd_temp);
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    if (!commHandle) {
        fprintf(stderr, "Plugin library \"lib%s-client.so\" not found\n", queryDesc->type.name);
        cleanUp(commHandle);
        return NULL;
    }

    // Considers the last path and loads the plugin interface
    substrStart = g_configData.pluginInterfaceDir;
    do{

        if(*substrStart == ':') substrStart++;

        substrEnd = strstr(substrStart, ":");
        if(substrEnd == NULL){
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }

        strcpy(cwd_temp, ""); // set empty string
        // Parse plugin interface file
        if(*folderPath != '/') { // if path is not absolute path; then consider edisonlib directory
            strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
        }

        strcat(cwd_temp, folderPath);
        strcat(cwd_temp, "/");
        strcat(cwd_temp, *(commHandle->interface));
        strcat(cwd_temp, ".json");
        #if DEBUG
            printf("\ninterface file name %s\n",cwd_temp);
        #endif
        if(fileExists(cwd_temp)){
            if (!parsePluginInterfaces(cwd_temp)) freeGlobals();
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);


    if(loadCommInterfaces(commHandle) == false)
        cleanUp(commHandle);

    if(commHandle->init){
        commHandle->init(queryDesc);
    }

    /*strcpy(cwd_temp, edisonlibcdir);
    strcat(cwd_temp, g_configData.pluginDir);
    strcat(cwd_temp, "/lib");
    strcat(cwd_temp, queryDesc->type.name);
    strcat(cwd_temp, "-client.so");
    commHandle = loadClientCommPlugin(cwd_temp);
    if (!commHandle) cleanUpClient(commHandle);
    commHandle->init(queryDesc);*/

    return commHandle;
}


/** Checks for file existence
 * @param[in] absPath absolute path of a file
 * @return returns true if the file exists and false otherwise
 */
bool fileExists(char *absPath)
{
    FILE *fp;
    if (fp = fopen(absPath, "r"))
    {
        fclose(fp);
        return true;
    }

    return false;
}

/** Initializes Service object. Creates the service object and calls its init method for further initialization
 * @param[in] description service description
 * @return returns service handle upon successful and NULL otherwise
 */
CommHandle *createService(ServiceDescription *description)
{
    CommHandle *commHandle = NULL;
    char cwd_temp[1024];

    // Parse configuration file
    strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
    strcat(cwd_temp, LIB_CONFIG_FILENAME);
    if (!parseConfigFile(cwd_temp)) freeGlobals();

// load the plugin
    // Considers the last path and loads the plugin interface
    char *substrStart = g_configData.pluginDir;
    char *substrEnd, *folderPath;
    do{
        if(*substrStart == ':') substrStart++;

        substrEnd = strstr(substrStart, ":");
        if(substrEnd == NULL){
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }

        strcpy(cwd_temp, ""); // set empty string

        // Parse plugin file
        if(*folderPath != '/'){ // if path is not absolute path; then consider edisonlib directory
            strcpy(cwd_temp, LIB_PLUGINS_DIRECTORY);
        }
//        strcat(cwd_temp, folderPath);
        strcat(cwd_temp, "lib");
        strcat(cwd_temp, description->type.name);
        strcat(cwd_temp, "-service.so");

        if(fileExists(cwd_temp)){
            commHandle = loadCommPlugin(cwd_temp);
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    if (!commHandle) {
        fprintf(stderr, "Plugin library \"lib%s-service.so\" not found\n", description->type.name);
        cleanUp(commHandle);
        return NULL;
    }

    // Considers the last path and loads the plugin interface
    substrStart = g_configData.pluginInterfaceDir;
    do{
        if(*substrStart == ':') substrStart++;

        substrEnd = strstr(substrStart, ":");
        if(substrEnd == NULL){
            folderPath = strdup(substrStart);
        } else {
            folderPath = strndup(substrStart, substrEnd - substrStart);
        }

        strcpy(cwd_temp, ""); // set empty string

        // Parse plugin interface file
        if(*folderPath != '/') { // if path is not absolute path; then consider edisonlib directory
            strcpy(cwd_temp, LIB_CONFIG_DIRECTORY);
        }
        strcat(cwd_temp, folderPath);
        strcat(cwd_temp, "/");
        strcat(cwd_temp, *(commHandle->interface));
        strcat(cwd_temp, ".json");

        if(fileExists(cwd_temp)){
            if (!parsePluginInterfaces(cwd_temp)) freeGlobals();
            break;
        }
    }while((substrStart = strstr(substrStart, ":")) != NULL);

    if(loadCommInterfaces(commHandle) == false)
            cleanUp(commHandle);

    if(commHandle->init){
        commHandle->init(description);
    }

    return commHandle;
}

void* commInterfacesLookup(CommHandle *commHandle, char *funcname){

    int i;

    for(i = 0; i < commHandle->interfacesCount; i ++){
        if(strcmp(commHandle->interfaces[i]->iname, funcname) == 0){
            return &(commHandle->interfaces[i]->iptr);
        }
    }

    return NULL;
}


#if DEBUG
int main(int argc, char *argv[])
{
    ServiceDescription *description = parseServiceDescription("../../sample-apps/serviceSpecs/temperatureServiceMQTT.json");
    CommHandle *commHandle = createClient(description);

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
