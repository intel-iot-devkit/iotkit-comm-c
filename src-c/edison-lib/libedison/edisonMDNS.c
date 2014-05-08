/*
 * MDNS Plugin to handle addition and removal of MDNS Service Records
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
#include <netdb.h>
#include <arpa/inet.h>
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

// discover context we passing around which contains function pointers to
// callback and Filter
typedef struct _DiscoverContext {
    bool (*filterCB)(ServiceQuery *, char *, uint16_t);
    void (*callback)(void *, int32_t, void *);
    void *serviceSpec;
} DiscoverContext;

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

        description->service_name = NULL;
        description->type.name = NULL;
        description->type.protocol = NULL;
        description->address = NULL;
        description->comm_params.ssl = NULL;
        description->properties = NULL;
        description->advertise.locally = NULL;
        description->advertise.cloud = NULL;

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

        jitem = cJSON_GetObjectItem(json, "address");
	    if (isJsonString(jitem)) {
            description->address = strdup(jitem->valuestring);
            #if DEBUG
            printf("host address %s\n", description->address);
            #endif
        } else {
            description->address = NULL;
        }
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

	    child = cJSON_GetObjectItem(json, "comm_params"); // this is an optional parameter; so, ignore if absent
        if (isJsonObject(child)){
            jitem = cJSON_GetObjectItem(child, "ssl"); // this is an optional parameter; so, ignore if absent
            if (isJsonString(jitem)){
                description->comm_params.ssl = strdup(jitem->valuestring);
            }
        }


        child = cJSON_GetObjectItem(json, "advertise"); // this is an optional parameter; so, ignore if absent
        if (isJsonObject(child)){
            jitem = cJSON_GetObjectItem(child, "locally"); // this is an optional parameter; so, ignore if absent
            if (isJsonString(jitem)){
                description->advertise.locally = strdup(jitem->valuestring);
            }

            jitem = cJSON_GetObjectItem(child, "cloud"); // this is an optional parameter; so, ignore if absent
            if (isJsonString(jitem)){
                description->advertise.cloud = strdup(jitem->valuestring);
            }
             #if DEBUG
                 printf("advertise locally=%s cloud=%s\n", description->advertise.locally,
                 description->advertise.cloud);
             #endif
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

char *getIPAddressFromHostName(char *hostname,char *PortAsNumber) {

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0; /* any type socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

   int s = getaddrinfo(hostname, PortAsNumber, &hints, &result);
   if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return NULL;
   }

	struct sockaddr_in *addr;
   	addr = (struct sockaddr_in *)result->ai_addr;

   	char *ipaddress = inet_ntoa((struct in_addr)addr->sin_addr);
   	#if DEBUG
	    printf("IP Address = %s\n",ipaddress);
	#endif

	return ipaddress;

}

// Handle events from DNS server
void handleEvents(DNSServiceRef client, void (*callback)(void *, int32_t, void *))
{
    #if DEBUG
        printf("\n in handleevents now\n");
    #endif
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

static void DNSSD_API discover_resolve_reply(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                    const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
{
    union {
    uint16_t s;
    u_char b[2];
    }port = { opaqueport };

    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

    #if DEBUG
        printf(stderr,"%s can be reached at %s:%u (interface %d)", fullname, hosttarget, PortAsNumber, ifIndex);
    #endif
    if (errorCode)
        fprintf(stderr,"Error code %d\n", errorCode);

    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceQuery *query = discContext->serviceSpec;

    // there is a filterCB, so calls it. If it returns false then donothing
    	if (discContext->filterCB && discContext->filterCB(query, fullname, PortAsNumber) == false)
    	    return;

    // check whether user has configured any host address
    if (query->address == NULL)
        query->address = hosttarget;

    query->port = PortAsNumber;
    discContext->callback(client, errorCode, createClient(query));
}

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
    ServiceDescription *desc = (ServiceDescription *) discContext->serviceSpec;
    DNSServiceErrorType err;
    
#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError)
    {
	if (flags & kDNSServiceFlagsAdd)
	    desc->status = ADDED;
	else
	    desc->status = REMOVED;

#if DEBUG
    printf("desc status %d\n", desc->status);
#endif

	/*// there is a filterCB, so calls it. If it returns false then donothing
	if (discContext->filterCB && discContext->filterCB(desc, client) == false)
	    return;*/
	err = DNSServiceResolve(&client, 0, interfaceIndex, name, regtype, domain, discover_resolve_reply, context);
	    if (!client || err != kDNSServiceErr_NoError)
        {
            sprintf(lastError, "queryReply DNSServiceResolve call failed %ld\n", (long int)err);
            discContext->callback(client, err, NULL);
            if (client)  {
                DNSServiceRefDeallocate(client);
            }
        }
        else
        {
            handleEvents(client,discContext->callback);
        }
    }
    else 
    {
	sprintf(lastError, "MDNS unexpected error");
	discContext->callback(client, errorCode, NULL);
    }
}

// Discover the service from MDNS. Filtered by the filterCB
void WaitToDiscoverServicesFiltered(ServiceQuery *queryDesc,
	    bool (*filterCB)(ServiceQuery *, char *, uint16_t),
	    void (*callback)(void *, int32_t, void *))
{
    
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    DiscoverContext *context = (DiscoverContext *)malloc(sizeof(DiscoverContext));
    if (!context) return;
    context->filterCB = filterCB;
    context->callback = callback;
    context->serviceSpec = queryDesc;
    

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

bool serviceQueryFilter(ServiceQuery *srvQry, char *fullname, uint16_t PortAsNumber){

    bool isNameMatched = false;
    bool isPortMatched = false;
    bool isPropertiesMatched = true; // TODO:

    // what we received from discover_resolve_reply is full name; so construct full name to compare an exact match (excluding the domain name)
    int testFullNameSize = strlen(srvQry->service_name) + strlen(srvQry->type.name) + strlen(srvQry->type.protocol) + 6; // adding 6 for ._ and NULL characters
    char *testfullName = (char *)malloc(sizeof(char) * testFullNameSize);

    strcpy(testfullName, srvQry->service_name);
    strcat(testfullName, "._");
    strcat(testfullName, srvQry->type.name);
    strcat(testfullName, "._");
    strcat(testfullName, srvQry->type.protocol);
    strcat(testfullName, ".");

    // Now that, we have constructed fullName from Service Query; check whether it matches with service name reported by discover_resolve_reply
    if(fullname == strstr(fullname, testfullName)){ // check whether
    #if DEBUG
        printf("Yes %s:matches with:%s\n", fullname, testfullName);
    #endif
        isNameMatched = true;
    }

    if(PortAsNumber == srvQry->port){
    #if DEBUG
        printf("Yes port:%u:matched with:%u\n", PortAsNumber, srvQry->port);
    #endif
        isPortMatched = true;
    }

    if(isNameMatched && isPortMatched && isPropertiesMatched){
    #if DEBUG
        printf("Returning TRUE --- Match found for service:%s\n", fullname);
    #endif
        return true;
    }


#if DEBUG
    printf("Returning FALSE --- No Match found for service:%s\n", fullname);
#endif
    return false;
}

// Discover the service from MDNS
void WaitToDiscoverServices(ServiceQuery *queryDesc,
	void (*callback)(void *, int32_t, void *) )
{
    WaitToDiscoverServicesFiltered(queryDesc, serviceQueryFilter, callback);
}

static void DNSSD_API advertise_resolve_reply(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                    const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
{
    union {
    uint16_t s;
    u_char b[2];
    }port = { opaqueport };

    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

    #if DEBUG
        printf("%s can be reached at %s:%u (interface %d)\n", fullname, hosttarget, PortAsNumber, ifIndex);
    #endif

    if (errorCode)
        printf(stderr,"advertise_resolve_reply Error code %d\n", errorCode);

    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceDescription *description = discContext->serviceSpec;
    // check whether user has explicitly specified the host address
    if (description->address != NULL) {
        char portarr[128];
        sprintf(portarr, "%d",description->port);
        char *ipaddress = getIPAddressFromHostName(description->address,portarr);
        if (ipaddress != NULL) {
            description->address = ipaddress;
    	    discContext->callback(client, errorCode,createService(description));
    	} else {
    	    printf("\nIn advertise_resolve_reply Host Name to IP Address Conversion Failed\n");
    	}
    } else {
        discContext->callback(client, errorCode,createService(description));
    }

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

    DiscoverContext *discContext = (DiscoverContext *)context;
    DNSServiceErrorType err;
    ServiceDescription desc;
    desc.service_name = (char *)name;

#if DEBUG
    printf("Got a reply for %s.%s.%s\n", name, regtype, domain);
#endif
    if (errorCode == kDNSServiceErr_NoError)
    {
	desc.status = REGISTERED;
	DNSServiceRef client1;
	err = DNSServiceResolve(&client1, 0, 0, name, regtype, domain, advertise_resolve_reply, context);
        if (!client1 || err != kDNSServiceErr_NoError)
        {
        sprintf(lastError, "regReply DNSServiceResolve call failed %ld\n", (long int)err);
        discContext->callback(client1, err, NULL);
        if (client)  {
            DNSServiceRefDeallocate(client1);
        }
        }
        else
        {
        handleEvents(client1,discContext->callback);
        }
    }
    else if (errorCode == kDNSServiceErr_NameConflict)
    {
        sprintf(lastError, "Name in use, please choose another %s.%s.%s", name, regtype, domain);
	desc.status = IN_USE;
	discContext->callback(client, errorCode, NULL);
    }
    else
    {
	sprintf(lastError, "MDNS unexpected error");
	discContext->callback(client, errorCode, NULL);
    }

}

// Advertise the service. Return an opaque object which is passed along to
// callback
void WaitToAdvertiseService(ServiceDescription *description,
	void (*callback)(void *, int32_t, void *))
{		
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    TXTRecordRef txtRecord;

    DiscoverContext *context = (DiscoverContext *)malloc(sizeof(DiscoverContext));
    if (!context) return;
    context->callback = callback;
    context->serviceSpec = description;

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
	    context);	    // param to pass as context into regReply

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
	WaitToAdvertiseService(description, callback);

    printf ("Done advertise\n");
}

// Test code for discover
void testDiscover()
{
    ServiceQuery *query = parseServiceDescription("../../sample-apps/serviceSpecs/temperatureServiceMQTT.json");
    if (query)
	WaitToDiscoverServices(query, callback);
    printf("Done discover\n");
}

int main(void) 
{
    //testAdvertise();
    testDiscover();
}

#endif
