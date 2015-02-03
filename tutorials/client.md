
Writing a client using iotkit-comm requires three fundamental steps:
    1) Write a service query. <BR>
    2) Create a client that queries for the service. <BR>
    3) Compile the client. <BR>
    4) Run the client. <BR>

<B> Write a service query </B>

A service query is a JSON string that specifies the attributes of the service a client wishes to connect to. In
this example, the service query similar to the service specification shown in the [service](@ref service.md)
tutorial. It is assumed that the service query is written into file client-query.json

    {
        "name": "/ndg/temperature/cpuTemp",
        "type": {
            "name": "zmqreqrep",
            "protocol": "tcp"
            },
        "properties": {"dataType": "float", "unit": "F"},
    }

<B> Create a client that queries for the service </B>

Now here's the source code for the client (place in file zmqreqrep-client.c):

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm/iotkit-comm.h"
#include "iotkit-comm/util.h"

ServiceQuery *query = NULL;

void reqMessageCallback(char *message, Context context) {
    fprintf(stderr,"Message received in Client: %s\n", message);
}

void reqDiscoveryCallback(void *handle, int32_t error_code, CommHandle *commHandle) {
    if (commHandle != NULL) {
        int (**send)(char *, Context context);
        int (**receive)(void (*)(char *, Context));
        Context context;
        int i = 0;

        send = commInterfacesLookup(commHandle, "send");
        receive = commInterfacesLookup(commHandle, "receive");
        if (send != NULL && receive != NULL) {
            while (i < 9) { // Event Loop
                (*send)("toys",context);
                (*receive)(reqMessageCallback);
                sleep(2);

                i ++;
            }

            // clean the service query object
            cleanUpService(&query, &commHandle);
            exit(0);
        } else {
            puts("Interface lookup failed");
        }
    } else {
        puts("\nComm Handle is NULL\n");
    }
}

int main(void) {
    puts("Sample program to test the iotkit-comm ZMQ req/rep plugin !!");
    query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryZMQREQREP.json");

    if (query) {
        discoverServicesBlocking(query, reqDiscoveryCallback);
    }

    return 0;
}

Notice that the client does not need to know the IP address of the service or even how to communicate with it. Instead,
the service query takes care of such details. For example, this client specifies the name of the service and the
protocol the service should be speaking (zmqreqrep). When the service is found, iotkit-comm returns a client object with an
appropriately initialized communication handle client.comm. This handle is an instance of the zmqreqrep plugin
connected to the service. More on communication plugins later, but for now a brief note,
on communication plugins is they provide functions like send and receive. The main difference
between the various communication plugins is how the send and receive happen (e.g. different packet format and headers).
Note that iotkit-comm comes bundled with a few default communication plugins, but its also easy to write your own if necessary.

<B> Compile the client </B>

$ cc -o client zmqreqrep-client.c -liotkit-comm -ldns_sd -ldl -lm <BR>

<B> Run the client </B>

$ ./client

<B> Learn More </B>

&bull; Write the corresponding [service](@ref service.md)  <BR>
&bull; Understand service [specifications and queries](@ref service-spec-query.md) (important) <BR>
&bull; Learn to write a more comprehensive [distributed application](@ref application.md)  <BR>
&bull; Understand and write [communication plugins](@ref plugin.md)  <BR>
