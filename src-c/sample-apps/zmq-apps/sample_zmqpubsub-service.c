#include <stdio.h>
#include <cJSON.h>
#include <stdbool.h>
#include <sys/types.h>
#include "edisonapi.h"
#include "util.h"

ServiceDescription *serviceDescription = NULL;

void callback(void *handle, int32_t error_code, CommServiceHandle *serviceHandle)
{
	    if (serviceHandle != NULL) {
            Context context;
            while(1) {
                serviceHandle->publish("vehicle: car",context);
                sleep(2);
            }
        } else {
            puts("\nComm Handle is NULL\n");
        }

}



int main(void) {

    void *handle;

	puts("Sample program to test the Edison ZMQ pub/sub plugin !!");
    serviceDescription = (ServiceDescription *) parseServiceDescription("./serviceSpecs/temperatureServiceZMQPUBSUB.json");

    if (serviceDescription)
	    handle = advertiseService(serviceDescription, callback);


    while(1);

	return 0;
}

