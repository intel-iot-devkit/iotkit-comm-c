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

/**
 * @file edisonMDNS.c
 * @brief Implementation of Edison MDNS
 *
 * Provides features to Advertise a Service, to Discover for the available Services with Filtering Mechanism
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <pthread.h>

#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include <cJSON.h>
#include <errno.h>              // For errno, EINTR
#include <dns_sd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>
#include "util.h"
#include "edisonMDNS.h"

#ifndef DEBUG
#define DEBUG 0
#endif

static uint32_t opinterface = kDNSServiceInterfaceIndexAny;
#define LONG_TIME 100000000
#define SHORT_TIME 10000
static volatile int timeOut = LONG_TIME;


static char lastError[256];

/* Helper method to return the last error
 */
char *getLastError() { return lastError; }


/* Helper snippet to handle JSON parse errors
 */
#define handleParseError() \
{\
    if (description) free(description);\
    description = NULL;\
    fprintf(stderr,"invalid JSON format for %s file\n", service_desc_file);\
    goto endParseSrvFile;\
}

/** Parses service description.
 * @param[in] service_desc_file file path to the service description JSON
 * @return returns service description object upon successful parsing and NULL otherwise
 */
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
        description->port = 0;
        description->comm_params.ssl = NULL;
        description->numProperties = 0;
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
            description->properties = (Property **)malloc(sizeof(Property *) * description->numProperties);
            i=0;
            child = jitem->child;
            while (child) {
                description->properties[i] = (Property *)malloc(sizeof(Property));

                description->properties[i]->key = strdup(child->string);
                description->properties[i]->value = strdup(child->valuestring);
                #if DEBUG
                    printf("properties key=%s value=%s\n", description->properties[i]->key,
                    description->properties[i]->value);
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

/** Parses client query description.
 * @param[in] service_desc_file file path to the query description JSON
 * @return returns client query description object upon successful parsing and NULL otherwise
 */
ServiceQuery *parseClientServiceQuery(char *service_desc_file)
{
    ServiceQuery *description = NULL;
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

	    description = (ServiceQuery *)malloc(sizeof(ServiceQuery));
	    if (description == NULL) {
		fprintf(stderr, "Can't alloc memory for service description\n");
		goto endParseSrvFile;
	    }

        description->service_name = NULL;
        description->type.name = NULL;
        description->type.protocol = NULL;

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

endParseSrvFile:
            cJSON_Delete(json);
        }

        // free buffers
	    fclose(fp);
        free(buffer);
    }
    return description;
}

/** Retrives IP Address for a given host name.
 * @param[in] hostname denotes the host name
 * @param[in] port denotes the port information
 * @return returns IP Address
 */
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

    // duplicate the IP address so that subsequent calls to 'inet_ntoa' does not overwrite the value
   	char *ipaddress = strdup(inet_ntoa((struct in_addr)addr->sin_addr));
   	#if DEBUG
	    printf("IP Address = %s\n",ipaddress);
	#endif

	return ipaddress;

}

/** Handle events from DNS Server
 * @param[in] client service ref object
 * @param[in] callback callback function to be invoked upon any DNS errors
 */
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

/** Callback invoked upon resolving a service from DNS.
 * @param[in] client service reference object
 * @param[in] flags DNS serivce flags
 * @param[in] ifIndex interface index
 * @param[in] errorCode error code
 * @param[in] fullservicename complete service name
 * @param[in] hosttarget host on which the service is available
 * @param[in] opaqueport port information
 * @param[in] txtLen lenght of TXT Record
 * @param[in] txtRecord TXT Record
 * @param[in] context context information
 */
static void DNSSD_API discover_resolve_reply(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
                                    const char *fullservicename, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
{
    union {
    uint16_t s;
    u_char b[2];
    }port = { opaqueport };

    uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

    #if DEBUG
        printf("%s can be reached at %s:%u (interface %d)", fullservicename, hosttarget, PortAsNumber, ifIndex);
    #endif
    if (errorCode)
        fprintf(stderr,"Error code %d\n", errorCode);

    DiscoverContext *discContext = (DiscoverContext *)context;
    ServiceQuery *query = discContext->serviceSpec;

    // perform service filter
    if(serviceQueryFilter(query, fullservicename, PortAsNumber, txtLen, txtRecord) == false)
        return;

    char *filteredServiceAddress = serviceAddressFilter(query, hosttarget, fullservicename, PortAsNumber);

    if(!filteredServiceAddress)
        return;

    // there is a user filterCB, so call it. If it returns false then donothing
    if (discContext->userFilterCB && discContext->userFilterCB(query) == false)
    	return;

    /*// check whether user has configured any host address
    if (query->address == NULL)
        query->address = hosttarget;*/

    query->address = filteredServiceAddress;
    query->port = PortAsNumber;
    #if DEBUG
        printf("\nquery->port: %d\n",query->port);
    #endif
    discContext->callback(client, errorCode, createClient(query));
}

/** Callback invoked upon discovering a query reply from DNS.
 * @param[in] client service reference object
 * @param[in] flags DNS serivce flags
 * @param[in] interfaceIndex interface index
 * @param[in] errorCode error code
 * @param[in] name service name
 * @param[in] regtype registered type
 * @param[in] domain domain information
 * @param[in] context context information
 */
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

/** Creates a client for the given ServiceDescription object
 * @param[in] queryDesc service description
 * @param[in] callback callback to be invoked upon successful client creation
 */
void createClientForGivenService(ServiceQuery *queryDesc, void (*callback)(void *, int32_t, void *)){
    if(queryDesc->address == NULL){
        queryDesc->address = "127.0.0.1"; // defaults to localhost
    }

    if(queryDesc->port == 0){
        fprintf(stderr, "Error: Unknown Port information\n");
        return;
    }

    callback(NULL, 0, createClient(queryDesc));
}
/** Browse or Discover a service from MDNS. This is a blocking call
 * @param[in] queryDesc service description
 * @param[in] userFilterCB callback method for user filter
 * @param[in] callback callback to be invoked upon successful client creation
 */
void WaitToDiscoverServicesFiltered(ServiceQuery *queryDesc,
	    bool (*userFilterCB)(ServiceQuery *),
	    void (*callback)(void *, int32_t, void *))
{
    
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    DiscoverContext *context = (DiscoverContext *)malloc(sizeof(DiscoverContext));

    setMyAddresses(); // initialize my IP Addresses


    if (!context) return;
    context->userFilterCB = userFilterCB;
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


/** Match the service name against user supplied service query by resolving regular expression (if any)
 * @param[in] srvQry service query
 * @param[in] fullservicename complete service name
 */
bool  getServiceNameMatched(ServiceQuery *srvQry, char *fullservicename) {
        regex_t regex;
        int res;
        char msgbuf[100];
        #if DEBUG
            printf("\nFull Service name %s\n",fullservicename);
        #endif
        // searching for character '.' where the service name ends
        char *end = strchr(fullservicename,'.');
        if (end == NULL) {
            printf ("searched character NOT FOUND\n");
            return false;
        }

        char servicename[256];
        int i = 0;

        while (fullservicename != end) {
            servicename[i++] = *fullservicename++;
        }
        servicename[i] = '\0';

        #if DEBUG
            printf("\nReal Service name %s\n",servicename);
        #endif

        /* Compile regular expression */
        res = regcomp(&regex, srvQry->service_name, REG_EXTENDED);
        if(res ) { fprintf(stderr, "Could not compile regex\n");}

        /* Execute regular expression */
        res = regexec(&regex, servicename, 0, NULL, 0);
        if( !res ) {
            return true;
        }
        else if(res == REG_NOMATCH ) {
            return false;
        }
        else {
            regerror(res, &regex, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        }

        /* Free compiled regular expression if you want to use the regex_t again */
	    regfree(&regex);

	    return false;
}


/** Address filtering for service available locally
 * @param[in] srvQry service query
 * @param[in] hosttarget target host name
 * @param[in] fullname full name of the service
 * @param[in] portAsNumber port information
 */
char* serviceAddressFilter(ServiceQuery *srvQry, const char *hosttarget, const char *fullname, uint16_t portAsNumber){

    if (!hosttarget || !fullname) {
        if (!fullname) {
          printf("WARN: Discovered a service without a name. Dropping.\n");
        } else {
          printf("WARN: Discovered a service without addresses. Dropping.\n");
        }
        return NULL;
      }

    char *serviceName;
    char portAsChar[256];
    sprintf(portAsChar, "%d", portAsNumber);

    if(strstr(fullname, "._")){
        //copy only the service name
        serviceName = strndup(fullname, strstr(fullname, "._") - fullname);
    } else {
        // looks like we actually got the service name instead of fullname
        serviceName = strdup(fullname);
    }

    char *address = getIPAddressFromHostName(hosttarget, portAsChar);

    ServiceCache *traverse = serviceCache;
    bool isPresentInCache = false;
    bool isServiceSeenBefore = false;
    while(traverse != NULL){
        if(strcmp(traverse->servicename, serviceName) == 0){

            if(strcmp(traverse->address, address) == 0){
                isPresentInCache = true;
            }else {
                // service is already known; but got callback on new address/interface
                isServiceSeenBefore = true;
            }
        }
        traverse = traverse->next;
    }

    if(isPresentInCache == true){
        return NULL;
    }

    ServiceCache *newService = (ServiceCache *)malloc(sizeof(ServiceCache));
    newService->next = NULL;
    newService->servicename = serviceName;
    newService->address = address;

    if(serviceCache == NULL){
        serviceCache = newService;
    } else {
        traverse = serviceCache;
        while(traverse->next != NULL){
            traverse = traverse->next;
        }

        traverse->next = newService;
    }

    if(isServiceSeenBefore){
        return NULL;
    }

    if(isServiceLocal(address)){
        return LOCAL_ADDRESS;
    }

    // return the IP address of non-local host
    return address;
}

/** Verifies whether the service is available locally or not
 * @param[in] address host address information
 * @return returns true if local address and false otherwise
 */
bool isServiceLocal(const char *address){
    int i;
    for(i = 0; i < myaddressesCount; i ++){
        if(strcmp(myaddresses[i], address) == 0){
            return true;
        }
    }

    return false;
}

/** Service Query Filter
 * @param[in] srvQry service query
 * @param[in] fullservicename complete service name
 * @param[in] PortAsNumber port information
 * @param[in] txtLen length of TXT Record
 * @param[in] txtRecord TXT Record
 */
bool serviceQueryFilter(ServiceQuery *srvQry, char *fullservicename, uint16_t PortAsNumber, uint16_t txtLen, const unsigned char *txtRecord){

    bool isNameMatched = false;
    bool isPortMatched = false;
    bool isPropertiesMatched = false;

    Property **properties;

    // check whether service name in ServiceQuery matches with service name reported by discover_resolve_reply
    if(getServiceNameMatched(srvQry,fullservicename)){ // check whether
    #if DEBUG
        printf("Yes %s:matches with:%s\n", fullservicename, srvQry->service_name);
    #endif
        isNameMatched = true;
    }

   if(srvQry->port){ // if port details present in discovery query
        if(PortAsNumber == srvQry->port){
        #if DEBUG
            printf("Yes port:%u:matched with:%u\n", PortAsNumber, srvQry->port);
        #endif
            isPortMatched = true;
        }
    } else {
        // port not defined by the service query; so consider as matched successfully
        isPortMatched = true;
    }
    
    uint16_t propertiesCountInTxtRecord = TXTRecordGetCount(txtLen, txtRecord);
    int bufferKeySize = 256;
    char bufferKey[bufferKeySize];
    void *bufferValue;
    uint8_t bufferValueSize;
    int i, j;

    properties = (Property **)malloc(sizeof(Property *) * propertiesCountInTxtRecord);
    for(i = 0; i < propertiesCountInTxtRecord; i ++){
        properties[i] = (Property *)malloc(sizeof(Property));

        TXTRecordGetItemAtIndex(txtLen, txtRecord, i, bufferKeySize -1, bufferKey, &bufferValueSize, &bufferValue);

        properties[i]->key = strdup(bufferKey);
        properties[i]->value = strndup(bufferValue, bufferValueSize);

        #if DEBUG
            printf("READ Property:%s:%s; from TXT Record\n", properties[i]->key, properties[i]->value);
        #endif
    }

    if(srvQry->numProperties > 0 && srvQry->properties != NULL){
        // look for atleast one property match
        for(i = 0; i < srvQry->numProperties; i ++){
            for(j = 0; j < propertiesCountInTxtRecord; j ++){
                if(strcmp(srvQry->properties[i]->key, properties[j]->key) == 0 && \
                    strcmp(srvQry->properties[i]->value, properties[j]->value) == 0){
                        isPropertiesMatched = true; // yes found atleast one matching property
                        break;
                    }
            }
        }
    } else {
        // there are no properties defined by the service query; so consider as matched successfully
        isPropertiesMatched = true;
    }

    if(isNameMatched && isPortMatched && isPropertiesMatched){
    #if DEBUG
        printf("Returning TRUE --- Match found for service:%s\n", fullservicename);
    #endif
        return true;
    }

#if DEBUG
    printf("Returning FALSE --- No Match found for query %s with service name :%s\n",srvQry->service_name,fullservicename);
#endif
    return false;
}

/** Browse or Discover a service from MDNS. This is a blocking call
 * @param[in] queryDesc service description
 * @param[in] callback callback to be invoked upon successful client creation
 */
void WaitToDiscoverServices(ServiceQuery *queryDesc,
	void (*callback)(void *, int32_t, void *) )
{
    WaitToDiscoverServicesFiltered(queryDesc, NULL, callback);
}

/** Callback invoked upon resolving an advertised service through DNS.
 * @param[in] client service reference object
 * @param[in] flags DNS serivce flags
 * @param[in] ifIndex interface index
 * @param[in] errorCode error code
 * @param[in] fullname complete service name
 * @param[in] hosttarget host on which the service is available
 * @param[in] opaqueport port information
 * @param[in] txtLen lenght of TXT Record
 * @param[in] txtRecord TXT Record
 * @param[in] context context information
 */
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

/** Callback invoked upon registering a service via DNS.
 * @param[in] client service reference object
 * @param[in] flags DNS serivce flags
 * @param[in] errorCode error code
 * @param[in] name service name
 * @param[in] regtype registered type
 * @param[in] domain domain information
 * @param[in] context context information
 */
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
	err = DNSServiceResolve(&client, 0, 0, name, regtype, domain, advertise_resolve_reply, context);
        if (!client || err != kDNSServiceErr_NoError)
        {
        sprintf(lastError, "regReply DNSServiceResolve call failed %ld\n", (long int)err);
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

/** Advertise a service. Return an opaque object.
 * @param[in] description service description
 * @param[in] callback callback to be invoked upon successful service creation
 */
bool advertiseService(ServiceDescription *description)
{
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    TXTRecordRef txtRecord;

    setMyAddresses(); // initialize my IP Addresses

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
	    txtLen = (uint8_t)strlen(description->properties[i]->value);
	    TXTRecordSetValue(&txtRecord, description->properties[i]->key,
				    txtLen, description->properties[i]->value );
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
	    NULL,			// callback
	    NULL);	    // param to pass as context into regReply

    if (description->numProperties)  {
	    TXTRecordDeallocate(&txtRecord);
    }

    if (!client || err != kDNSServiceErr_NoError)
    {
        sprintf(lastError, "DNSServiceRegister call failed %ld\n", (long int)err);
        if (client)
            DNSServiceRefDeallocate(client);

        return false;
    }

    return true;
}


/** Advertise a service. Return an opaque object which is passed along to callback.
 * Note: This is a blocking call
 * @param[in] description service description
 * @param[in] callback callback to be invoked upon successful service creation
 */
void WaitToAdvertiseService(ServiceDescription *description,
	void (*callback)(void *, int32_t, void *))
{		
    DNSServiceRef client;
    DNSServiceErrorType err;
    pthread_t tid;	    // thread to handle events from DNS server
    char regtype[128];
    TXTRecordRef txtRecord;

    setMyAddresses(); // initialize my IP Addresses

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
	    txtLen = (uint8_t)strlen(description->properties[i]->value);
	    TXTRecordSetValue(&txtRecord, description->properties[i]->key,
				    txtLen, description->properties[i]->value );
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

/** Initialize local addresses on various network interfaces
 */
bool setMyAddresses(void){

    int iSocket;
    struct if_nameindex *if_ni, *i;
    int j;

    if ((iSocket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
      perror("socket");
      return false;
    }

    if_ni = if_nameindex();

    // verify how many addresses do we have for the network interfaces
    for (i = if_ni, myaddressesCount; ! (i->if_index == 0 && i->if_name == NULL); i++){
        struct ifreq req;
        strncpy(req.ifr_name, i->if_name, IFNAMSIZ);
        if (ioctl(iSocket, SIOCGIFADDR, &req) < 0)
        {
            if (errno == EADDRNOTAVAIL)
            {
                continue;
            }
            perror("ioctl");
            close(iSocket);
            return false;
        }
        myaddressesCount ++;
    }

    myaddresses = (char **)malloc(sizeof(char *) * myaddressesCount);

    for (i = if_ni, j = 0; ! (i->if_index == 0 && i->if_name == NULL); i++)
    {
        struct ifreq req;
        strncpy(req.ifr_name, i->if_name, IFNAMSIZ);
        if (ioctl(iSocket, SIOCGIFADDR, &req) < 0)
        {
            if (errno == EADDRNOTAVAIL)
            {
                myaddresses[j] = NULL;
                continue;
            }
            perror("ioctl");
            close(iSocket);
            return false;
        }

        myaddresses[j++] = strdup(inet_ntoa(((struct sockaddr_in*)&req.ifr_addr)->sin_addr));
        #if DEBUG
            printf("Got IP Address:%s\n", myaddresses[j-1]);
        #endif
    }
    if_freenameindex(if_ni);
    close(iSocket);

    return true;
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
