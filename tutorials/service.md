
Writing a service using iotkit-comm requires three fundamental steps:

1) Write a service specification. <BR>
2) Create a service based on that specification. <BR>
3) Compile the service. <BR>
4) Run the service. <BR>

<B> Write a service specification </B>

A service specification is a JSON string that describes a service. It specifies attributes such as the port on which
the service will run, the name of the service, the protocol it will use to communicate etc. Here is the specification
for the service we will be writing (place in file server-spec.json):

    {
        "name": "/ndg/temperature/cpuTemp",
        "type": {
            "name": "zmqreqrep",
            "protocol": "tcp"
            },
        "port": 18333,
        "properties": {"dataType": "float", "unit": "F"},
    }

<B> Create a service based on that specification </B>

Now here's the source code for the service itself (place in file zmqreqrep-service.c):

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "iotkit-comm/iotkit-comm.h"
#include "iotkit-comm/util.h"

ServiceSpec *serviceSpec = NULL;

void repMessageCallback(void *client, char *message, Context context) {
    fprintf(stderr,"Message received in Server: %s\n", message);
}

void repAdvertiseCallback(void *handle, int32_t error_code,CommHandle *serviceHandle) {
    if (serviceHandle != NULL) {
        void *client = NULL;
        Context context;
        void (**sendTo)(void *, char *, Context context);
        int (**receive)(void (*)(void *, char *, Context context));
        int i = 0;

        sendTo = commInterfacesLookup(serviceHandle, "sendTo");
        receive = commInterfacesLookup(serviceHandle, "receive");
        if (sendTo != NULL && receive != NULL) {
            while(i < 10) {  // Event Loop
                (*sendTo)(client,"train bike car",context);
                (*receive)(repMessageCallback);
                sleep(2);

                i ++;
            }

            // clean the service specification object
            cleanUpService(&serviceSpec, &serviceHandle);
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
    serviceSpec = (ServiceSpec *) parseServiceSpec("./server-spec.json");

    if (serviceSpec) {
        advertiseServiceBlocking(serviceSpec, repAdvertiseCallback);
    }

    return 0;
}

Notice that the service does not need to worry about how messages will be delivered, it only needs to worry about the
contents of those messages. Specifying zmqreqrep in the type.name field of the specification is enough to let iotkit-comm
know how to send messages. The underlying communication details are handled by communication plugins; in this case,
the communication plugin is an instance of the zmqreqrep plugin. More on communication plugins later, but for now a brief note,
on communication plugins is they provide functions like send and receive. The main difference
between the various communication plugins is how the send and receive happen (e.g. different packet format and headers).
Note that iotkit-comm comes bundled with a few default communication plugins, but its also easy to write your own if necessary.

<B> Compile the service </B>

$ cc -o server zmqreqrep-service.c -liotkit-comm -ldns_sd -ldl -lm <BR>

<B> Run the service </B>

$ ./server

<B> Learn More </B>

&bull; Write the corresponding [client](@ref client.md)  <BR>
&bull; Understand service [specifications and queries](@ref service-spec-query.md)(important)  <BR>
&bull; Learn to write a more comprehensive [distributed application](@ref application.md)  <BR>
&bull; Understand and write [communication plugins](@ref plugin.md)  <BR>
