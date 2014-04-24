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

typedef union { unsigned char b[2]; unsigned short NotAnInteger; } Opaque16;
static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
static int operation = 'R';
#define LONG_TIME 100000000
#define SHORT_TIME 10000
static volatile int timeOut = LONG_TIME;

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
    int numentries=0, i=0;
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

            jitem = cJSON_GetObjectItem(child, "subtypes");
	    if (!isJsonArray(jitem)) handleParseError();
		    
	    child = jitem->child;
	    while (child) numentries++, child=child->next;
	    description->type.subTypes = (char **)malloc(numentries*sizeof(char*));

	    child = jitem->child;
	    while (child) {
		description->type.subTypes[i] = strdup(child->valuestring);
	        #if DEBUG
	        printf("subType %s\n", description->type.subTypes[i]);
	        #endif
		i++;
		child=child->next;
	    }

	    jitem = cJSON_GetObjectItem(json, "port");
	    if (!isJsonNumber(jitem)) handleParseError();

	    description->port = jitem->valueint;
	    #if DEBUG
	    printf("port %d\n", description->port);
	    #endif

	    jitem = cJSON_GetObjectItem(json, "properties");
	    if (!isJsonObject(jitem)) handleParseError();

            description->properties = cJSON_Print(jitem, 0);
	    #if DEBUG
	    printf("properties %s\n", description->properties);
	    #endif

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
    void (*callback)(void *, bool status, char *) 
	= (void (*)(void *, bool status, char *))context;
    char message[1024];

#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError)
    {
        sprintf(message, "Name now registered and active %s.%s.%s", name, regtype, domain);
	callback(client, true, message);
    }
    else if (errorCode == kDNSServiceErr_NameConflict)
    {
        sprintf(message, "Name in use, please choose another %s.%s.%s", name, regtype, domain);
	callback(client, false, message);
    }
    else 
    {
	sprintf(message, "Error MDNS code %d\n", errorCode);
	callback(client, false, message);
    }

}

void *handleEvents(void *c)
{
    DNSServiceRef client = (DNSServiceRef)c;
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
		fprintf(stderr, "DNSServiceProcessResult returned %d\n", err); 
		stopNow = 1; 
	    }
        }
        else if (result == 0)
        {
	    DNSServiceErrorType err = DNSServiceProcessResult(client);
            if (err != kDNSServiceErr_NoError)
            {
                fprintf(stderr, "DNSService call failed %ld\n", (long int)err);
                stopNow = 1;
            }
        }
        else
        { 
	    printf("select() returned %d errno %d %s\n", result, errno, strerror(errno));
            if (errno != EINTR) 
		stopNow = 1;
        }
    }
}


// Advertise the serivce
void *advertiseService(ServiceDescription *description,
		       void (*callback)(void *, bool, char *) ) 
{		
    DNSServiceRef client;
    Opaque16 registerPort = { { 0x7, 0x5B } };
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char msg[128];
    char regtype[128];
    static const char TXT[] = "\xC" "First String" "\xD" "Second String" "\xC" "Third String";

    // register type
    strcpy(regtype, "_");
    strcat(regtype, description->type.name);
    strcat(regtype, "._");
    strcat(regtype, description->type.protocol); 

    printf("txt description  = %s\n", TXT);

    err = DNSServiceRegister
		(&client, 
		0, 
		opinterface, 
		description->service_name,   // service name
		regtype,		// registration type
		"",			// domain (null = pick sensible default = local)
		NULL,	    // only needed when creating proxy registrations for services
		registerPort.NotAnInteger, 
		sizeof(TXT)-1, //	description->properties ? strlen(description->properties) : 0,  // text size
		TXT, // description->properties,	// text description
		regReply,		// callback
		callback);

    if (!client || err != kDNSServiceErr_NoError) 
    {
	sprintf(msg, "DNSServiceRegister call failed %ld\n", (long int)err);
	callback(client, false, msg);
	if (client) 
	    DNSServiceRefDeallocate(client);
	client = NULL;
    }
    else 
    {
	// Create a thread to handle events
        if (pthread_create(&tid, NULL, &handleEvents, (void *)client) != 0)
	{
	    strcpy(msg, "Can't create thread to handle events from DNS server");
	    callback(client, false, msg);
	    if (client ) DNSServiceRefDeallocate(client );
	    client = NULL;
	}
    }

    return client;
}

#if DEBUG
void callback(void *handle, bool status, char *msg)
{
    printf("message %s\n", msg);
}

int main(void) 
{
    void *handle;
    ServiceDescription *description = parseServiceDescription("./serviceSpecs/temperatureService.json");
    if (description)
	handle = advertiseService(description, callback);    

    printf ("Done advertise\n");
    while(1);
}

#endif
