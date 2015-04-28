Publishing and subscribing to data from the cloud requires that you:

1) Create a cloud account <BR>
2) Publish data <BR>
3) View the published data <BR>
4) Subscribe to the data <BR>
5) Troubleshoot (if necessary) <BR>

<B> Create a cloud account </B>

Go to Intel's enableiot cloud site [https://dashboard.us.enableiot.com] and follow instructions to create an account.
Once an account is created, an activation key will be available in the account details section. Record this
activation key; note that it will *expire* in less than *one hour*. If your activation key has expired,
you can always create a new one by clicking on the adjacent 'refresh' button.

<B> Publish data </B>

Create a service specification for your sensor (garage-sensor-spec.json):

    {
        "name" : "temperature.v1.0/garage_sensor",
        "type" : {
            "name": "enableiot"
        },
        "port": 34562,
        "type_params": {"deviceid":"EdisonInGarage", "activationCode":"<<YOUR DEVICE ACTIVATION CODE>>"}
    }

Write the code to publish data:

    #include <stdio.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include "iotkit-comm/iotkit-comm.h"
    #include "iotkit-comm/util.h"

    ServiceSpec *srvSpec = NULL;
    int msgnumber = 13; // sensor value

    void callback(void *handle, int32_t error_code, void *serviceHandle) {
        Context context;
        char msg[256];
        int i = 0;
        CommHandle *commHandle = NULL;

        if(serviceHandle != NULL) {
            commHandle = (CommHandle *) serviceHandle;

            int (**publish) (char *message,Context context);

            publish = commInterfacesLookup(commHandle, "publish");
            if(publish == NULL) {
                fprintf(stderr, "Function \'publish\' is not available; please verify the Plugin documentation !!\n");
                return;
            }

            context.name = "topic";
            context.value = "data";

            while(i < 10) {  // Event Loop
                sprintf(msg, "{\"name\": \"garage_sensor\", \"value\": %d}", msgnumber++);
                printf("Publishing msg:%s\n", msg);

                (*publish)(msg, context);
                sleep(2);

                i ++;
            }
        }

        // clean the objects
        cleanUpService(&srvSpec, &commHandle);
        exit(0);
    }

    int main(void) {
        puts("Sample program to publish data to IoT Cloud !!");

        srvSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceEnableIot.json");

        if (srvSpec){
            advertiseServiceBlocking(srvSpec, callback);
        }

        return 0;
    }


It is important to know that data can only be published using sensors. The cloud supports two types of sensors by
default: temperature (temperature.v1.0) and humidity (humidity.v1.0). You may create other types by going to the
account details page; then, clicking on the 'Catalog' tab; and then finally, clicking on the 'Add a New
Catalog Item' button. These need to be specified as the second-last path element in the 'name' field of
the specification. The last path element is the friendly name of the sensor, e.g. 'garage_sensor'.
As soon as the service is started, the sensor 'garage_sensor' is registered with the cloud and data
can be published as necessary.

<B> View the published data </B>

Go to Intel's enableiot cloud site [https://dashboard.us.enableiot.com], login, click on the "Menu" button,
and click "Charts". Then, select your device and sensor-type to see a graph of your published data vs.
time.

<B> Subscribe to data </B>

To receive data published by the sample 'garage_sensor' service running on 'EdisonInGarage', create a
service query ('garage-sensor-query.json'):

    {
        "name" : "temperature.v1.0/garage_sensor",
        "type" : {
            "name": "enableiot"
        },
        "type_params": {"deviceid": "EdisonInGarage", "activationCode":"<<YOUR DEVICE ACTIVATION CODE>>", "subscribeto": "EdisonInGarage", "frequencyInterval": 5}
    }

Then, write the code to subscribe to the data:

    #include <stdio.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include "iotkit-comm/iotkit-comm.h"
    #include "iotkit-comm/util.h"

    ServiceQuery *query = NULL;
    CommHandle *commHandle = NULL;
    int i = 0;

    void message_callback(char *message, Context context) {
        printf("Message received:%s\n", message);

        i ++;

        if(i >= 3) {
            // clean the objects
            cleanUpService(&query, &commHandle);
            exit(0);
        }
    }

    int serviceStarted = 0;

    void callback(void *handle, int32_t error_code, void *serviceHandle) {
        if(serviceHandle != NULL && !serviceStarted) {
            commHandle = (CommHandle *) serviceHandle;
            char *response = NULL;
            int (**receive)(void (*)(char *, Context)) = NULL;

            receive = commInterfacesLookup(commHandle, "receive");
            if(receive == NULL) {
                printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
                return;
            }

            (*receive)(message_callback);

            while(1) { // Infinite Event Loop
                sleep(1);
            }
        }
    }

    bool serviceFilter(ServiceQuery *srvQuery) {
        printf("Got into Service Filter\n");
        return true;
    }

    int main(void) {

        puts("Sample program to test the IoT Cloud subscribe plugin !!");
        query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperatureServiceQueryEnableIot.json");

        if (query) {
            createClientForGivenService(query, callback);
        }

        return 0;
    }


<B> Troubleshooting </B>

Some of the most common issues stem from not being connected to the network. We suggest running the following
command on your Edison to ensure that you are connected to the cloud:

    curl www.intel.com/edison

If this command hangs or fails with an error, it means the Edison is not connected to the Internet.

NOTE: please ensure that the activation key has not expired; you can always create a new one as described in the
'Create a cloud account' section above.
