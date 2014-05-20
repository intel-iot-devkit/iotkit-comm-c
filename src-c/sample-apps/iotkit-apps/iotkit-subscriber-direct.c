/*
 ============================================================================
 Name        : edison-mqtt_async.c
 Author      : pc
 Version     :
 Copyright   : Your copyright notice
 Description : Asynchronous MQTT client
 ============================================================================
 */

#include "iotkit-subscriber-direct.h"
#include "dlfcn.h"

#include <signal.h>

void *handle=NULL;
char *err=NULL;

MQTTAsync client;
MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;


void handleSignal(int sig)
 {
 	toStop = 1;
 	printf("got interruption signal");
 }

 int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
 {
 	int i;
 	char* payloadptr;
 	if((sent++ % 1000) == 0)
 		printf("%d messages received\n", sent++);

 	printf("Message arrived\n");
 	printf("topic: %s\n", topicName);
 	printf("message:");

 	payloadptr = message->payload;
 	for(i=0; i<message->payloadlen; i++)
 	{
 		putchar(*payloadptr++);
 	}
 	putchar('\n');

 	MQTTAsync_freeMessage(&message);
 	MQTTAsync_free(topicName);
 	return 1;
 }

 void onSubscribe(void* context, MQTTAsync_successData* response)
 {
 	printf("Subscribe succeeded\n");
 }

 void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
 {
 	printf("Subscribe failed\n");
 	finished = 1;
 }

 void onDisconnect(void* context, MQTTAsync_successData* response)
 {
 	printf("Successful disconnection\n");
 	finished = 1;
 }


 void onSendFailure(void* context, MQTTAsync_failureData* response)
 {
	printf("onSendFailure: message with token value %d delivery failed\n", response->token);
 }



 void onSend(void* context, MQTTAsync_successData* response)
 {
	static last_send = 0;

	if (response->token - last_send != 1)
		printf("Error in onSend, token value %d, last_send %d\n", response->token, last_send);

	last_send++;

 	if ((response->token % 1000) == 0)
 		printf("onSend: message with token value %d delivery confirmed\n", response->token);
 }

 void deliveryComplete(void* context, MQTTAsync_token token)
 {
 	sent++;
 	if ((sent % 1000) == 0)
 		printf("deliveryComplete: message with token value %d delivery confirmed\n", token);
	if (sent != token)
		printf("Error, sent %d != token %d\n", sent, token);
//	if (sent == options.message_count)
//		toStop = 1;
 }

 void onConnectFailure(void* context, MQTTAsync_failureData* response)
 {
 	printf("Connect failed\n");
 	if(response != NULL){
		printf("token:%d", response->token);
		printf("code:%d", response->code);
		printf("message:%s", response->message);
 	}
 	finished = 1;
 }


 void onConnect(void* context, MQTTAsync_successData* response)
 {
	printf("Successful Connected\n");
 	connected=1;
 }

 void connectionLost(void *context, char *cause)
 {
 	MQTTAsync client = (MQTTAsync)context;
 	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
 	int rc;

 	printf("\nConnection lost\n");
 	printf("     cause: %s\n", cause);

 	printf("Reconnecting\n");
 	conn_opts.keepAliveInterval = 20;
 	conn_opts.cleansession = 1;
 	conn_opts.onSuccess = onConnect;
 	conn_opts.onFailure = onConnectFailure;
 	conn_opts.context = client;
 	conn_opts.retryInterval = 1000;
 	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
 	{
 		printf("Failed to start connect, return code %d\n", rc);
 		finished = 1;
 	}
 }

 int publish(char *topic, char *message) {

 	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
 	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

 	int rc = 0;

 	unsigned long i;
 	 		struct timeval tv;
 	 		gettimeofday(&tv,NULL);
 	 		printf("start seconds : %ld\n",tv.tv_sec);

 	 			opts.onSuccess = onSend;
 				opts.onFailure = onSendFailure;
 	 			opts.context = client;
 	 			pubmsg.payload = message;
 	 			pubmsg.payloadlen = strlen(message);
 	 			pubmsg.qos = QOS;
 	 			pubmsg.retained = 0;
 	 			deliveredtoken = 0;
 	 			usleep(100);

 	 			if ((rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts))
 	 				!= MQTTASYNC_SUCCESS)
 	 			{
 	 				printf("Failed to start sendMessage, return code %d\n", rc);
 	 				exit(-1);
 	 			}


 	 		gettimeofday(&tv,NULL);

 	 		printf("end seconds : %ld\n",tv.tv_sec);

 	return rc;
 }


 int subscribe(char *topic, void (*callback)(char *topic, char *message)) {
 	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

 	int rc = 0;

 	opts.onSuccess = onSubscribe;
 	opts.onFailure = onSubscribeFailure;
 	opts.context = client;

 	if ((rc = MQTTAsync_subscribe(client, topic, QOS, &opts)) != MQTTASYNC_SUCCESS) {
 	 	printf("Failed to subscribe, return code %d\n", rc);
 	 	exit(-1);
 	}

 	while (!finished)
 	{
 	 	usleep(1000L);

 	 	if (toStop == 1)
 	 	{
 	 		close123();
 	 	}
 	}

 	return rc;
 }


 int close123() {

 	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;

 	int rc = 0;

 	opts.onSuccess = onDisconnect;
 	opts.context = client;

 	if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
 	{
 		printf("Failed to start disconnect, return code %d\n", rc);
 	 	exit(-1);
 	}
 	finished = 1;
 	toStop = 0;

 	MQTTAsync_destroy(&client);

 	return rc;
 }


void handleTrace(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
	printf("%s\n", message);
}


 //values for type --> open, ssl
 int createSubscriptionClient(char *host, int port, char *type, void *sslargs)
{
		MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;
	int rc = 0;
	 	char uri[256];

	 	sprintf(uri, "tcp://%s:%d", host, port);
		// Default settings:
	 	int i=0;



	 	MQTTAsync_token token;

	 	signal(SIGINT, handleSignal);
	 	signal(SIGTERM, handleSignal);

	 	quietMode = 0;


	 	printf("URL IS:%s \n", uri);

		MQTTAsync_setTraceCallback(handleTrace);                                                 
		MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_PROTOCOL);


	 	MQTTAsync_create(&client, strdup(uri), strdup(CLIENTID), MQTTCLIENT_PERSISTENCE_NONE, NULL);

//		MQTTAsync_setTraceCallback(handleTrace);
//		MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_ERROR);


	 	MQTTAsync_setCallbacks(client, client, connectionLost, messageArrived, deliveryComplete);

	 	conn_opts.cleansession = 0;
	 	conn_opts.onSuccess = onConnect;
	 	conn_opts.onFailure = onConnectFailure;
	 	conn_opts.context = client;
	 	conn_opts.keepAliveInterval = 20;
	 	conn_opts.retryInterval = 0;

//	 	conn_opts.username = "testuser1";
//	 	conn_opts.password = "testpassword1";

	 	//conn_opts.maxInflight= 30;


	 	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	 	{
	 		printf("Failed to start connect, return code %d\n", rc);
	 		exit(1);
	 	}
	 	printf("Waiting for connect\n");
	 	while (connected == 0 && finished == 0 && toStop == 0) {
	 		printf("Waiting for connect: %d %d %d\n", connected, finished, toStop);
	 		sleep(1);
	 	}


    return rc;
}


 int main(void) {
 	puts("Sample program to test the Edison MQTT pub/sub plugin !!");

 	createSubscriptionClient("dev-broker.us.enableiot.com", 1883, "open", NULL);

 	subscribe("/server/metric/43d7606c-4f07-4f3b-958a-974c4a403039/f0-de-f1-e4-75-bb", NULL);

 	return EXIT_SUCCESS;
 }
