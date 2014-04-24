/*
 * zmq-pubsub.h
 *
 *  Created on: 2014-04-18
 *      Author: skothurx
 */

#ifndef ZMQ_PUBSUB_H_
#define ZMQ_PUBSUB_H_

int send(char *topic, char *message);
int subscribe(char *topic, void (*callback)(char *topic, char *message));
int unsubscribe(char *topic);
int setReceivedMessageHandler();
//values for type --> open, ssl
int createClient(char *host, int port, char *type, void *sslargs);
int createService();
int done();

#endif /* ZMQ_PUBSUB_H_ */
