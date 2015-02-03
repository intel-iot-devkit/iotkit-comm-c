
Service specifications and queries are the key entities in understanding how iotkit-comm library simplifies the development of
distributed applications. A distributed application written using iotkit-comm is composed essentially of clients and services.

When creating a service, the developer:

1) Writes a service specification. <BR>
2) Requests the iotkit-comm library to create a service based on that specification. <BR>

When creating a client, the developer:

1) Writes a service query. <BR>
2) Requests iotkit-comm to find and connect to a service whose attributes match that query. <BR>

Service specifications and queries look similar. The fundamental difference is that a service query is used
by clients whereas a service specification is used by services: a specification is used to initialize a service while a
query is used to find a service.

At this point, we will digress a little and explain what it means to find a service on the network. The act of finding
a service involves querying for it (by name, the protocol it uses, etc.) and getting the corresponding hostname and
port number in return. Now, a service can only be found if it is first advertised on the network. One can think of
advertising a service as constantly broadcasting the corresponding service information on the LAN.
When one of these service information "matches" a service query then we say that a service has been found.
The protocol that takes care of advertising service specifications and matching them to service queries is called mDNS
and you can learn more about it [here](http://en.wikipedia.org/wiki/Multicast_DNS).
Please note that service specifications, queries, and matching are iokit-comm abstractions for underlying mDNS concepts.
So you should not expect to find these terms in documents describing mDNS.

We will now explain how to write service specifications and queries. Before we get into the details though, it is
important to note that both service specifications and queries are JSON strings that fully or partially describe the
attributes of a service.

<B> Service Specification </B>

Here is a sample service specification:

    {
        "name": "/my/home/thermostat/temperature_sensor",
        "type": {
            "name": "zmqpubsub",
            "protocol": "tcp"
        },
        "port": 8999,
        "properties": {"dataType": "float", "unit": "F", "sensorType": "ambient"}
    }

Let's go through each of the above attributes:

a) name (<I>compulsory</I>): a string, preferably a user-friendly one, since service names might be displayed to user by
applications <BR>
b) type (<I>compulsory</I>) <BR>
    &ensp; &bull; name (<I>compulsory</I>): name of the protocol this service will be speaking. Here, 'zmqpubsub' implies that the
        service will be using zeromq pub/sub sockets to communicate. More specifically, a zeromq publisher is a
        service that writes to sockets of type pub. A zeromq subscriber is a client that can then "subscribe" to data
        written to that socket. The iotkit-comm library supports other protocols like mqtt and zmqreqrep that are implemented
        as "plugins". You are not required to use supported communication protocols, but it is quite convenient to do
        so. More on this in the [Plugin](@ref plugin.md) tutorial. <BR>
    &ensp; &bull; protocol (<I>compulsory</I>): this is the transport protocol; only 'tcp' is supported <BR>
c) port (<I>optional</I>): port number the service will run on. If unspecified the library will select a free port automatically <BR>
d) properties (<I>optional</I>): any user defined properties the service has. Each property must be a "name": value pair.
Here, the properties indicate that the sensor is publishing the ambient temperature in Fahrenheit using a
floating-point format. More on these properties when we talk about the thermostat. <BR>

This specification can then be passed to advertiseServiceBlocking() or advertiseService(), which will eventually advertise
the service information along with hostname and port details.

<B> Service Query </B>

Here is a sample service query:

     {
        "name": ".*temperature_sensor$",
        "type": {
            "name": "zmqpubsub",
            "protocol": "tcp"
        },
        "type_params": {
            "mustsecure": true
        }
     }

Notice that this query matches the sample specification in the previous section. Additionally, this query will find
all temperature sensors on the LAN that speak the zmqpubsub protocol. With that in mind, let's go through each of the
above attributes:

a) name (<I>compulsory</I>): can be specified as a regular expression <BR>
b) type (<I>compulsory</I>) <BR>
    &ensp; &bull; name (<I>compulsory</I>): the higher level communication protocol the service uses. This allows the client to find
        only those services with which it can communicate. For example, a zmq subscriber will find only zmq
        publishers if it specifies zmqpubsub in the type.name field of the query (see zeromq pub/sub sockets). <BR>
    &ensp; &bull; protocol (<I>compulsory</I>): the transport layer protocol (only tcp is supported) <BR>
c) type_params (<I>optional</I>): as mentioned above, the details of how messages are sent and received are handled by
communication plugins. These plugins abstract out communication details and allow the developer to focus on message
contents instead of how messages are exchanged. The plugins may support configuration parameters that a developer can
set; these parameters are passed in using the type_params field. A communication plugin will document the parameters
it accepts. For example, in the specification above, the field type_params contains the mustsecure parameter which states
that a secure connection should should be used (note: The above specification is only an example). <BR>

This query can then be passed to module:main.createClient, which will eventually return a client instance connected
to the corresponding service. Now, here is a more detailed service query:

    {
        "name": "/my/home/thermostat/temperature_sensor",
        "type": {
            "name": "zmqpubsub",
            "protocol": "tcp"
            },
        "properties": {"dataType": "float", "unit": "F", "sensorType": "ambient"},
        "type_params": {"mustsecure": false}
    }

Notice that it looks similar to the specification in previous section. If a client is aware of a service's address and
port number then the client need not search for the service, it can connect to it directly by using the function
createClientForGivenService(). Also, notice the type_params field which
tells the zmqpubsub plugin to make an insecure connection to the found service.
