#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <pthread.h>

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

static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
#define LONG_TIME 100000000
#define SHORT_TIME 10000
static volatile int timeOut = LONG_TIME;

// Last error message
static char lastError[256];
char *getLastError() { return lastError; }

// helper define
#define handleParseError() \
{\
    if (description) free(description);\
    description = NULL;\
    fprintf(stderr,"invalid JSON format for %s file\n", service_desc_file);\
    goto endParseSrvFile;\
}

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

static void DNSSD_API regReply(DNSServiceRef client, 
				const DNSServiceFlags flags, 
				DNSServiceErrorType errorCode,
				const char *name, 
				const char *regtype, 
				const char *domain, 
				void *context)
{
    (void)flags;    // Unused

    // get error callback
    void (*callback)(void *, int32_t, ServiceDescription *) 
	= (void (*)(void *, int32_t, ServiceDescription *))context;
    ServiceDescription desc;
    desc.service_name = (char *)name;

#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError)
    {
	desc.status = REGISTERED;
	callback(client, errorCode, &desc);
    }
    else if (errorCode == kDNSServiceErr_NameConflict)
    {
        sprintf(lastError, "Name in use, please choose another %s.%s.%s", name, regtype, domain);
	desc.status = IN_USE;
	callback(client, errorCode, &desc);
    }
    else 
    {
	sprintf(lastError, "MDNS unexpected error");
	callback(client, errorCode, NULL);
    }

}

// Handle events from DNS server
void handleEvents(DNSServiceRef client, void (*callback)(void *, int32_t, ServiceDescription *)) 
{
    int dns_sd_fd  = client  ? DNSServiceRefSockFD(client) : -1;
    int nfds = dns_sd_fd + 1;
    fd_set readfds;
    struct timeval tv;
    int result, stopNow = 0;

    while (!stopNow)
    {
	// 1. Set up the fd_set as usual here.
        FD_ZERO(&readfds);

        // 2. Add the fd for our client(s) to the fd_set
        if (client ) 
	    FD_SET(dns_sd_fd , &readfds);

        // 3. Set up the timeout.
        tv.tv_sec  = timeOut;
        tv.tv_usec = 0;

        result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
        #if DEBUG
	printf("select result = %d\n", result);
	#endif
        if (result > 0)
        {
	    DNSServiceErrorType err = kDNSServiceErr_NoError;
            if (client  && FD_ISSET(dns_sd_fd , &readfds)) 
		err = DNSServiceProcessResult(client );
            if (err) { 
		sprintf(lastError, "Failed waiting on DNS file descriptor");
		callback(client, err, NULL);
		stopNow = 1; 
	    }
        }
        else if (result == 0)
        {
	    DNSServiceErrorType err = DNSServiceProcessResult(client);
            if (err != kDNSServiceErr_NoError)
            {
                sprintf(lastError, "DNSService call failed");
		callback(client, err, NULL);
                stopNow = 1;
            }
        }
        else
        { 
	    sprintf(lastError, "select() returned %d errno %s", result, strerror(errno));
	    callback(client, errno, NULL);
            if (errno != EINTR) 
		stopNow = 1;
        }
    }
}

// discover context we passing around which contains function pointers to 
// callback and Filter 
typedef struct _DiscoverContext {
    bool (*filterCB)(ServiceDescription *);
    void (*callback)(void *, int32_t, ServiceDescription *);
} DiscoverContext;

// handle query reply
static void DNSSD_API queryReply(DNSServiceRef client, 
				DNSServiceFlags flags, 
				uint32_t interfaceIndex,
				DNSServiceErrorType errorCode,
				const char *name, 
				const char *regtype, 
				const char *domain, 
				void *context)
{
    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceDescription desc;

#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError)
    {
	if (flags & kDNSServiceFlagsAdd)
	    desc.status = ADDED;
	else
	    desc.status = REMOVED;
	desc.service_name = (char *)name;

	// there is a filterCB, so calls it. If it returns false then donothing
	if (discContext->filterCB && discContext->filterCB(&desc) == false)
	    return;
	discContext->callback(client, errorCode, &desc);
    }
    else 
    {
	sprintf(lastError, "MDNS unexpected error");
	discContext->callback(client, errorCode, NULL);
    }
}

// Discover the service from MDNS. Filtered by the filterCB
void discoverServicesFiltered(ServiceQuery *queryDesc, 
	    bool (*filterCB)(ServiceDescription *), 
	    void (*callback)(void *, int32_t, ServiceDescription *))
{
    
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    DiscoverContext *context = (DiscoverContext *)malloc(sizeof(DiscoverContext));
    if (!context) return;
    context->filterCB = filterCB;
    context->callback = callback;
    

    // register type
    strcpy(regtype, "_");
    strcat(regtype, queryDesc->type.name);
    strcat(regtype, "._");
    strcat(regtype, queryDesc->type.protocol); 

    err = DNSServiceBrowse
	    (&client, 
	    0, 
	    opinterface, 
	    regtype,    // registration type
	    "",		// domain (null = pick sensible default = local)
	    queryReply, // callback
	    context);	// param to pass as context into queryReply

    if (!client || err != kDNSServiceErr_NoError) 
    {
	sprintf(lastError, "DNSServiceBrowse call failed %ld\n", (long int)err);
	callback(client, err, NULL);
	if (client) 
	    DNSServiceRefDeallocate(client);
    }
    else 
    {
	handleEvents(client, callback);
    }
}

// Discover the service from MDNS
void discoverServices(ServiceQuery *queryDesc, 
	void (*callback)(void *, int32_t, ServiceDescription *) )
{
    discoverServicesFiltered(queryDesc, NULL, callback);
}

// Advertise the service. Return an opaque object which is passed along to
// callback
void advertiseService(ServiceDescription *description,
	void (*callback)(void *, int32_t, ServiceDescription *)) 
{		
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    TXTRecordRef txtRecord;

    // register type
    strcpy(regtype, "_");
    strcat(regtype, description->type.name);
    strcat(regtype, "._");
    strcat(regtype, description->type.protocol); 

    if (description->numProperties) 
    {
	uint8_t txtLen, i=0;
	TXTRecordCreate(&txtRecord, 0, NULL);
	for (i=0; i<description->numProperties; i++) 
	{
	    txtLen = (uint8_t)strlen(description->properties[i].value);
	    TXTRecordSetValue(&txtRecord, description->properties[i].key, 
				    txtLen, description->properties[i].value );
	}
    }

    err = DNSServiceRegister
	    (&client, 
	    0, 
	    opinterface, 
	    description->service_name,  // service name
	    regtype,			// registration type
	    "",				// default = local
	    NULL,	    // only needed when creating proxy registrations
	    htons(description->port),   // Must have a port
	    TXTRecordGetLength(&txtRecord), 
	    TXTRecordGetBytesPtr(&txtRecord),
	    regReply,			// callback
	    callback);	    // param to pass as context into regReply

    if (description->numProperties)  {
	TXTRecordDeallocate(&txtRecord);
    }

    if (!client || err != kDNSServiceErr_NoError) 
    {
	sprintf(lastError, "DNSServiceRegister call failed %ld\n", (long int)err);
	callback(client, err, NULL);
	if (client) 
	    DNSServiceRefDeallocate(client);
    }
    else 
    {
	handleEvents(client, callback);
    }
}

#if DEBUG
void callback(void *handle, int32_t error_code, ServiceDescription *desc)
{
    printf("message error=%d error_string=%s\nservice status=%d service name=%s\n", 
	    error_code, 
	    getLastError(),
	    desc ? desc->status : -1, 
	    desc ? desc->service_name : "");
}

// Test code for advertisement
void testAdvertise() 
{
    ServiceDescription *description = parseServiceDescription("../../sample-apps/serviceSpecs/temperatureServiceMQTT.json");
    if (description)
	advertiseService(description, callback);

    printf ("Done advertise\n");
}

// Test code for discover
void testDiscover()
{
    ServiceQuery *query = parseServiceDescription("../../sample-apps/serviceSpecs/temperatureServiceMQTT.json");
    if (query)
	discoverServices(query, callback);
    printf("Done discover\n");
}

int main(void) 
{
    //testAdvertise();
    testDiscover();
}

#endif
