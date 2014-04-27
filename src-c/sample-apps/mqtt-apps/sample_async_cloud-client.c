#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include "edisonapi.h"
#include "util.h"

void callback(char *message, Context context){
    printf("Message received:%s", message);
}

// helper define
#define handleParseError() \
{\
    if (description) free(description);\
    description = NULL;\
    fprintf(stderr,"invalid JSON format for %s file\n", service_desc_file);\
    goto endParseSrvFile;\
}

// TODO: Move Service Description to common module
// parse the service description
ServiceDescription *parseServiceDescription(char *service_desc_file)
{
    ServiceDescription *description = NULL;
    char *out;
    int i=0;
    cJSON *json, *jitem, *child;
    bool status = true;

    FILE *fp = fopen(service_desc_file, "rb");
    if (fp == NULL) {
        fprintf(stderr,"Error can't open file %s\n", service_desc_file);
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
	if (!json) {
            fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
	}
	else
	{
            #if DEBUG
            out = cJSON_Print(json, 2);
            printf("%s\n", out);
            free(out);
            #endif

            if (!isJsonObject(json)) handleParseError();

	    description = (ServiceDescription *)malloc(sizeof(ServiceDescription));
	    if (description == NULL) {
		fprintf(stderr, "Can't alloc memory for service description\n");
		goto endParseSrvFile;
	    }

	    // initially set status to UNKNOWN
	    description->status = UNKNOWN;

            jitem = cJSON_GetObjectItem(json, "name");
	    if (!isJsonString(jitem)) handleParseError();

            description->service_name = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("service name %s\n", description->service_name);
	    #endif

            child = cJSON_GetObjectItem(json, "type");
	    if (!isJsonObject(child)) handleParseError();

	    jitem = cJSON_GetObjectItem(child, "name");
	    if (!isJsonString(jitem)) handleParseError();

	    description->type.name = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("type name %s\n", description->type.name);
	    #endif

            jitem = cJSON_GetObjectItem(child, "protocol");
	    if (!isJsonString(jitem)) handleParseError();

            description->type.protocol = strdup(jitem->valuestring);
	    #if DEBUG
	    printf("protocol %s\n", description->type.protocol);
	    #endif

/*
// TODO: NOT HANDLING SUBTYPES
            jitem = cJSON_GetObjectItem(child, "subtypes");
	    if (!isJsonArray(jitem)) handleParseError();

	    description->type.numSubTypes = 0;
	    child = jitem->child;
	    while (child) description->type.numSubTypes++, child=child->next;
	    if (description->type.numSubTypes)
	    {
		description->type.subTypes = (char **)malloc(
					description->type.numSubTypes*sizeof(char*));
		i=0;
		child = jitem->child;
		while (child) {
		    description->type.subTypes[i] = strdup(child->valuestring);
		    #if DEBUG
		    printf("subType %s\n", description->type.subTypes[i]);
		    #endif
		    i++;
		    child=child->next;
		}
	    }
*/
	    // must have a port
	    jitem = cJSON_GetObjectItem(json, "port");
	    if (!jitem || !isJsonNumber(jitem)) handleParseError();
	    description->port = jitem->valueint;
	    #if DEBUG
	    printf("port %d\n", description->port);
	    #endif

	    jitem = cJSON_GetObjectItem(json, "properties");
	    if (!isJsonObject(jitem)) handleParseError();

	    description->numProperties = 0;
	    child = jitem->child;
	    while (child) description->numProperties++, child=child->next;
	    if (description->numProperties)
	    {
		description->properties = (Property *)malloc(
					    description->numProperties*sizeof(Property));
		i=0;
		child = jitem->child;
		while (child) {
		    description->properties[i].key = strdup(child->string);
		    description->properties[i].value = strdup(child->valuestring);
		    #if DEBUG
		    printf("properties key=%s value=%s\n", description->properties[i].key,
			    description->properties[i].value);
		    #endif
		    i++;
		    child=child->next;
		}
	    }

endParseSrvFile:
            cJSON_Delete(json);
        }

        // free buffers
	fclose(fp);
        free(buffer);
    }
    return description;
}

int main(void) {

CommClientHandle *commHandle;
	puts("Sample program to test the Edison MQTT pub/sub plugin !!");

    ServiceQuery *query = (ServiceQuery *) parseServiceDescription("../serviceSpecs/temperatureServiceMQTT.json");

	commHandle = createClient(query);

    commHandle->setReceivedMessageHandler(callback);
    commHandle->subscribe("/foo");
//	publish();

//	subscribe("/foo", NULL);

	//return EXIT_SUCCESS;
	return 0;
}

