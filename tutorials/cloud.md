
To publish data to the cloud and later subscribe to it, you will need to:

1) Create a cloud account <BR>
2) Add device in your cloud account <BR>
3) Activate your device in the cloud <BR>
4) Publish data from your device <BR>
5) Subscribe to data <BR>

<B> Create a cloud account </B>

Go to Intel's enableiot cloud site [https://dashboard.us.enableiot.com] and follow instructions to create an account.
Once an account is created, an activation key will be available in the account details section which we will be using
shortly.

<B> Add device in your cloud account </B>

Adding a device involves:

a) Getting your device's ID <BR>
b) Use the ID to register your device with the cloud

<B> Get your device's ID </B>

You can get the device id by running the following command in device terminal:

$ iotkit-admin device-id <BR>

<B> Use the ID to register your device with the cloud </B>

When you log in to your account, the first page you see is the dashboard. Click on 'Devices' menu option; then, click on
the 'Add New Device' button. Enter appropriate details about your device; if you do not have a gateway id or don't
know what that means, just enter the device ID in this field.
Once the device is added click on 'Account' menu option and note the 'Activation Code'. Proceed to the next
step: Activate your device in the cloud .

<B> Activate your device in the cloud </B>

Before you connect your device, you should first test if it can reach the cloud:

$ iotkit-admin test <BR>

If there are no errors, do the following:

$ iotkit-admin activate [your-activation-key] <BR>

If there are no errors, your device is activated and ready to publish data.

<B> Publish data from your device </B>

This section assumes that you know how to write a server application using iotkit-comm. If not, please go through the server
tutorial first.

Create a service specification for the cloud (temperatureServiceIoTKit.json):

    {
        "name" : "temperature.v1.0/garage_sensor",
        "type" : {
            "name": "iotkit-agent"
        },
        "port" : 1884,
        "type_params": {"mustsecure": false}
    }


Write the code to publish data:

    #include <stdio.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include "iotkit-comm/iotkit-comm.h"
    #include "iotkit-comm/util.h"

    ServiceSpec *srvSpec = NULL;
    int msgnumber = 40;

    void callback(void *handle, int32_t error_code, void *serviceHandle)
    {
        Context context;
        char msg[256];
        int i = 0;
        CommHandle *commHandle = NULL;

        if(serviceHandle != NULL)
        {
            commHandle = (CommHandle *) serviceHandle;

            int (**send) (char *message,Context context);

            send = commInterfacesLookup(commHandle, "publish");
            if(send == NULL)
            {
                printf("Function \'publish\' is not available; please verify the Plugin documentation !!\n");
                return;
            }

            context.name = "topic";
            context.value = "data";

            while(i < 10) {  // Event Loop
                sprintf(msg, "{\"n\": \"garage_sensor\", \"v\": %d}", msgnumber++);
                printf("Publishing msg:%s\n", msg);

                (*send)(msg, context);
                sleep(2);

                i ++;
            }
        }

        // clean the objects
        cleanUpService(&srvSpec, &commHandle);
        exit(0);
    }

    int main(void)
    {
        puts("Sample program to publish data to IoT Cloud !!");

        srvSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperatureServiceIoTKit.json");

        if (srvSpec){
            advertiseServiceBlocking(srvSpec, callback);
        }

        return 0;
    }


Note above, that data can only be published using sensors. The cloud supports two types of sensors by default
temperature (temperature.v1.0) and humidity (humidity.v1.0). You may create other types by going to the account
details account details page; then, clicking on the 'Catalog' tab; and then finally, clicking on the 'Add a New
Catalog Item' button.

After you've registered the sensor you can publish a reading or observation by using 'send' interface of
CommHandle *commHandle. Make sure the observation is a valid JSON object that contains both the n (name) and v
(value) fields. Also, note that the topic under which the observation is published is "data". This is currently the
only topic supported by the cloud.

<B> Subscribe to data </B>

This section assumes that you know how to write a client application using iotkit-comm. If not, please go through the client
tutorial first.

Create a service query for the cloud (temperatureServiceQueryIoTKit.json):

    {
        "name" : "temperature.v1.0/garage_sensor",
        "type" : {
            "name": "iotkit-agent"
        },
        "type_params": {"deviceid": "<<device ID>>"}
    }


Write the code to subscribe to the data:

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
            int (**receive)(void (*)(char *, Context)) = NULL;

            receive = commInterfacesLookup(commHandle, "receive");
            if(receive == NULL) {
                printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
                return;
            }

            (*receive)(message_callback);

            serviceStarted = 1;
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
        query = (ServiceQuery *) parseServiceQuery("./temperatureServiceQueryIoTKit.json");

        if (query) {
            createClientForGivenService(query, callback);
        }

        return 0;
    }

To receive data you must first subscribe to it. Note that it subscribes to the device ID specified in the query json.
