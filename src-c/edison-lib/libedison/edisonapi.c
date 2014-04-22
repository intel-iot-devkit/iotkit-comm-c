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

// current working dir
char *g_cwd;

// configuration data
typedef struct _ConfigFileData {
    char *pluginInterfaceDir;
    char *pluginDir;
    char *plugin;
} ConfigFileData;

ConfigFileData g_configData;

// function signatures
char **g_funcSignatures;

/* Parse config file */
bool parseConfigFile(char *config_file)
{
    char *out;
    cJSON *json, *jitem, *child;
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

            if (isJsonObject(json)) 
	    {
                jitem = cJSON_GetObjectItem(json, "pluginInterfaceDir");
                if (isJsonString(jitem)) 
		{
                    g_configData.pluginInterfaceDir = strdup(jitem->valuestring);
                    #if DEBUG
                    printf("pluginInterfaceDir = %s\n", g_configData.pluginInterfaceDir);
                    #endif
                }    
                                                                
                jitem = cJSON_GetObjectItem(json, "pluginDir");
                if (isJsonString(jitem)) {
                    g_configData.pluginDir = strdup(jitem->valuestring);
                    #if DEBUG
                    printf("pluginDir = %s\n", g_configData.pluginDir);
                    #endif
                }    

                jitem = cJSON_GetObjectItem(json, "plugins");
                if (isJsonArray(jitem)) 
		{
		    child=jitem->child;
                    while (child) 
		    {
			jitem = child;
                        child=child->next;
                    }

		    if (jitem) 
		    {
			jitem = cJSON_GetObjectItem(jitem, "fileName");
			if (isJsonString(jitem)) 
			{
			    g_configData.plugin = strdup(jitem->valuestring);
			    #if DEBUG
			    printf("plugin = %s\n", g_configData.plugin);
			    #endif
			}
			else 
			{
			    fprintf(stderr,"invalid plugin\n");
			    status = false;
			}
		    }
		    else 
		    {
			fprintf(stderr,"invalid %s file\n", config_file);
			status = false;
		    }
                }
            }
            else 
	    {
                fprintf(stderr,"invalid JSON format for %s file\n", config_file);
                status = false;
            }

            cJSON_Delete(json);
        }

	// free buffers
        free(buffer);
    }

    return status;
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

            if (isJsonObject(json)) 
	    {
                jitem = cJSON_GetObjectItem(json, "functions");
                if (isJsonArray(jitem)) 
		{
                    child = jitem->child;
                    /* How many entries in the array? */
                    while (child) numentries++,child=child->next; 
                    if (numentries) 
                    {
                        g_funcSignatures = (char **)malloc(numentries*sizeof(char*));
                        if (g_funcSignatures) 
			{
                            memset(g_funcSignatures,0,numentries*sizeof(char*));
                            child=jitem->child;
                            while (child)
                            {
                                if (isJsonString(child)) 
				{
                                    g_funcSignatures[i] = strdup(child->valuestring);    
                                    child = child->next;
                                    #if DEBUG
                                    printf("g_funcSignature = %s\n", g_funcSignatures[i]);
                                    #endif
                                    i++;
                                }
                                else 
				{
                                    fprintf(stderr,"invalid JSON format for %s file\n", inf_file);
                                    status = false;
                                    break;
                                }
                            }
                        } 
                        else 
			{
                            fprintf(stderr,"can't allocate memory\n");
                            status = false;
                        }
                    }
                    else 
		    {
                        fprintf(stderr,"invalid JSON format for %s file\n", inf_file);
                        status = false;
                    }
                }    
		else
		{
                    fprintf(stderr,"invalid JSON format for %s file\n", inf_file);
                    status = false;
                } 
            }
            else {
                fprintf(stderr,"invalid JSON format for %s file\n", inf_file);
                status = false;
            }

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

// check signatures and load the plugin
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
	    return NULL; 
	}
	else
	{
	    dlerror();	/* Clear any existing error */
	    commHandle->send = (void (*)(char *, char *)) dlsym(handle, g_funcSignatures[0]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->unsubscribe = (void (*)(char *)) dlsym(handle, g_funcSignatures[1]);
	    if (!checkDLError()) return NULL;

	    dlerror();	/* Clear any existing error */
	    commHandle->subscribe = (void (*)(char *)) dlsym(handle, g_funcSignatures[2]);
	    if (!checkDLError()) return NULL;
	}

	return commHandle;
    }
}

// initialize the system
CommHandle *createClient()
{
    CommHandle *commHandle;

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
    strcat(cwd_temp, "communication-pubsub.json");
    if (!parsePluginInterfaces(cwd_temp)) freeGlobals();

    // load the plugin
    strcpy(cwd_temp, g_cwd);
    strcat(cwd_temp, g_configData.pluginDir);
    strcat(cwd_temp, "/");
    strcat(cwd_temp, g_configData.plugin);
    commHandle = loadCommPlugin(cwd_temp);
    if (!commHandle) cleanUp(commHandle);

    return commHandle;
}    
    
#if DEBUG
int main(int argc, char *argv[])
{
    CommHandle *commHandle = createClient();
    if (commHandle) {
	commHandle->send("temperature", "75 degree");
	commHandle->unsubscribe("temperature");
	commHandle->subscribe("temperature");
    }
}
#endif
