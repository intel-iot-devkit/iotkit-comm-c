#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

#include <cJSON.h>
#include <errno.h>              // For errno, EINTR
#include <dns_sd.h>
#include <sys/types.h>
#include <sys/time.h>


#include "util.h"
#include "edisonapi.h"

#ifndef DEBUG
#define DEBUG 0
#endif

// helper define
#define handleParseError() \
{\
    status = false;\
    fprintf(stderr,"invalid JSON format for %s file\n", service_desc_file);\
    goto endParseSrvFile;\
}

// parse the service record
bool advertiseServices(char *service_desc_file) 
{
    ServiceRecord *record = NULL;
    char *out;
    int numentries=0, i=0;
    cJSON *json, *jitem, *child;
    bool status = true;
    FILE *fp = fopen(service_desc_file, "rb");

    if (fp == NULL) 
    {
        fprintf(stderr,"Error can't open file %s\n", service_desc_file);
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

            if (!isJsonObject(json)) handleParseError();

	    record = (ServiceRecord *)malloc(sizeof(ServiceRecord));
	    if (record == NULL) {
		fprintf(stderr, "Can't alloc memory for service record\n");
		goto endParseSrvFile;
	    }

            jitem = cJSON_GetObjectItem(json, "name");
	    if (!isJsonString(jitem)) handleParseError();
		    
            record->service_name = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("service name %s\n", record->service_name);
	    #endif

            child = cJSON_GetObjectItem(json, "type");
	    if (!isJsonObject(child)) handleParseError();
		    
	    jitem = cJSON_GetObjectItem(child, "name");
	    if (!isJsonString(jitem)) handleParseError();

	    record->type.name = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("type name %s\n", record->type.name);
	    #endif

            jitem = cJSON_GetObjectItem(child, "protocol");
	    if (!isJsonString(jitem)) handleParseError();
		    
            record->type.protocol = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("protocol %s\n", record->type.protocol);
	    #endif

            jitem = cJSON_GetObjectItem(child, "subtypes");
	    if (!isJsonArray(jitem)) handleParseError();
		    
	    child = jitem->child;
	    while (child) numentries++, child=child->next;
	    record->type.subTypes = (char **)malloc(numentries*sizeof(char*));

	    child = jitem->child;
	    while (child) {
		record->type.subTypes[i] = strdup(child->valuestring);
	        #if DEBUG
	        printf("subType %s\n", record->type.subTypes[i]);
	        #endif
		i++;
		child=child->next;
	    }

	    jitem = cJSON_GetObjectItem(json, "port");
	    if (!isJsonNumber(jitem)) handleParseError();

	    record->port = jitem->valueint;
	    #if DEBUG
	    printf("port %d\n", record->port);
	    #endif

	    jitem = cJSON_GetObjectItem(json, "properties");
	    if (!isJsonObject(jitem)) handleParseError();

            record->properties = cJSON_Print(jitem, 0);
	    #if DEBUG
	    printf("properties %s\n", record->properties);
	    #endif

endParseSrvFile:
            cJSON_Delete(json);
        }

        // free buffers
	fclose(fp);
        free(buffer);
    }

    return status;
}

#if DEBUG
int main(int argc, char *argv[])
{
    advertiseServices("../serviceSpecs/temperatureService.json");
}
#endif
