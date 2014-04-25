#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <dlfcn.h>

#include <cJSON.h>

#include "util.h"
#include "edisonapi.h"

#ifndef DEBUG
#define DEBUG 0
#endif

static inline bool checkDLError() {
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "DL error %s\n", error);
        return false;
    }
    return true;
}

// current working dir
char *g_cwd;

// configuration data
typedef struct _ConfigFileData {
    char *pluginInterfaceDir;
    char *pluginDir;
    char *clientFileSuffix;
    char *serverFileSuffix;
    char *plugin;
} ConfigFileData;

ConfigFileData g_configData;

// function signatures
char **g_funcSignatures;

// helper define
#define handleParseConfigError() \
{\
    status = false;\
    fprintf(stderr,"invalid JSON format for %s file\n", config_file);\
    goto endParseConfig;\
}

/* Parse config file */
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

            // TODO: We are considering only the last plugin specified under plugins array
            jitem = cJSON_GetObjectItem(json, "communication");
            if (!isJsonObject(json)) handleParseConfigError();

            jitem = cJSON_GetObjectItem(jitem, "plugins");
            if (!isJsonArray(jitem)) handleParseConfigError();

            child=jitem->child;
            while (child) 
	    {
		jitem = child;
                child=child->next;
            }

	    if (!jitem) handleParseConfigError();
            jitem = cJSON_GetObjectItem(jitem, "pluginName");
	    if (!isJsonString(jitem)) handleParseConfigError();
	    g_configData.plugin = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("plugin = %s\n", g_configData.plugin);
	    #endif

endParseConfig:
            cJSON_Delete(json);
        }

	// free buffers
        free(buffer);
    }

    return status;
}

// helper define
#define handleParseInterfacesError() \
{\
    status = false;\
    fprintf(stderr,"invalid JSON format for %s file\n", inf_file);\
    goto endParseInterfaces;\
}

// parse the plugin interfaces
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

// Free all the memory allocated for global data
void freeGlobals()
{
    if (g_funcSignatures) {
	free(g_funcSignatures);
    }
}

// clean up by freeing memory
void cleanUpClient(CommClientHandle *commHandle)
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

// clean up by freeing memory
void cleanUpService(CommServiceHandle *commHandle)
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


// check signatures and load the service plugin
CommServiceHandle *loadServiceCommPlugin(char *plugin_path)
{
    char *ptr;
    void *handle;
    CommServiceHandle *commHandle = (CommServiceHandle *)malloc(sizeof(CommServiceHandle));
    if (commHandle == NULL) 
    {
	fprintf(stderr,"Can't alloc memory for commHandle\n");
	return NULL;
    }
    else
    {
	// Check to see if filepath has the right extension as ".so"
	if ((ptr = strrchr(plugin_path, '.')) != NULL) 
	{
	    if (strcmp(ptr, ".so") != 0) 
	    {
		fprintf(stderr, "Invalid plugin file %s\n", plugin_path);
		*ptr = '\0';
		strcat(plugin_path, ".so");
	    }
	}
	else 
	{
	    strcat(plugin_path, ".so");
	}

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
	    commHandle->sendTo = (int (*)(void *, char *, Context)) dlsym(handle, g_funcSignatures[0]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->publish = (int (*)(char *,Context)) dlsym(handle, g_funcSignatures[1]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->manageClient = (int (*)(void *,Context)) dlsym(handle, g_funcSignatures[2]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->setReceivedMessageHandler = (int (*)(void (*)(void *, char *, Context))) dlsym(handle, g_funcSignatures[3]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->done = (int (*)()) dlsym(handle, g_funcSignatures[4]);
	    if (!checkDLError()) return NULL;

	    commHandle->handle = handle;
	}

	return commHandle;
    }
}

// check signatures and load the client plugin
CommClientHandle *loadClientCommPlugin(char *plugin_path)
{
    char *ptr;
    void *handle;
    CommClientHandle *commHandle = (CommClientHandle *)malloc(sizeof(CommClientHandle));
    if (commHandle == NULL)
    {
	fprintf(stderr,"Can't alloc memory for commHandle\n");
	return NULL;
    }
    else
    {
	// Check to see if filepath has the right extension as ".so"
	if ((ptr = strrchr(plugin_path, '.')) != NULL)
	{
	    if (strcmp(ptr, ".so") != 0)
	    {
		fprintf(stderr, "Invalid plugin file %s\n", plugin_path);
		*ptr = '\0';
		strcat(plugin_path, ".so");
	    }
	}
	else
	{
	    strcat(plugin_path, ".so");
	}

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
	    commHandle->send = (int (*)(char *, Context)) dlsym(handle, g_funcSignatures[0]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->subscribe = (int (*)(char *)) dlsym(handle, g_funcSignatures[1]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->unsubscribe = (int (*)(char *)) dlsym(handle, g_funcSignatures[2]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->setReceivedMessageHandler = (int (*)(void (*)(char *, Context))) dlsym(handle, g_funcSignatures[3]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->done = (int (*)()) dlsym(handle, g_funcSignatures[4]);
	    if (!checkDLError()) return NULL;

	    commHandle->handle = handle;
	}

	return commHandle;
    }
}

// initialize the system
CommClientHandle *createClient(ServiceQuery *queryDesc)
{
    CommClientHandle *commHandle;

    char cwd_temp[1024];
    // get current working directory
    if (getcwd(cwd_temp, sizeof(cwd_temp))) 
    {
        strcat(cwd_temp, "/");
        g_cwd = strdup(cwd_temp); 
    } 
    else 
    {
        g_cwd = strdup("./");
    }

    // Parse configuration file
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, "config.json");
    if (!parseConfigFile(cwd_temp)) freeGlobals();

    // Parse plugin interface file
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, g_configData.pluginInterfaceDir);
    strcat(cwd_temp, "/");
    strcat(cwd_temp, "edison-client-interface.json");
    if (!parsePluginInterfaces(cwd_temp)) freeGlobals();

    // load the plugin
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, g_configData.pluginDir);
    strcat(cwd_temp, "/lib");
    strcat(cwd_temp, queryDesc->type.name);
    commHandle = loadClientCommPlugin(cwd_temp);
    if (!commHandle) cleanUpClient(commHandle);

    return commHandle;
}

// initialize the system
CommServiceHandle *createService(ServiceDescription *description)
{
    CommServiceHandle *commHandle;

    char cwd_temp[1024];
    // get current working directory
    if (getcwd(cwd_temp, sizeof(cwd_temp)))
    {
        strcat(cwd_temp, "/");
        g_cwd = strdup(cwd_temp);
    }
    else
    {
        g_cwd = strdup("./");
    }

    // Parse configuration file
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, "config.json");
    if (!parseConfigFile(cwd_temp)) freeGlobals();

    // Parse plugin interface file
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, g_configData.pluginInterfaceDir);
    strcat(cwd_temp, "/");
    strcat(cwd_temp, "edison-service-interface.json");
    if (!parsePluginInterfaces(cwd_temp)) freeGlobals();

    // load the plugin
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, g_configData.pluginDir);
    strcat(cwd_temp, "/lib");
    strcat(cwd_temp, description->type.name);
    commHandle = loadServiceCommPlugin(cwd_temp);
    if (!commHandle) cleanUpService(commHandle);

    return commHandle;
}
    
#if DEBUG
int main(int argc, char *argv[])
{
    ServiceDescription *description = parseServiceDescription("../../sample-apps/serviceSpecs/temperatureServiceMQTT.json");
    CommClientHandle *commHandle = createClient(description);

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
