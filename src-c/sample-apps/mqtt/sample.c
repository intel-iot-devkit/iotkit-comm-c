#include "edison_pubsub.h"

int main(void) {
	puts("Sample program to test the Edison MQTT pub/sub plugin !!");

	createClient("192.168.10.3", 1883, "open", NULL);

//	publish();

	subscribe("/foo", NULL);

	return EXIT_SUCCESS;
}
