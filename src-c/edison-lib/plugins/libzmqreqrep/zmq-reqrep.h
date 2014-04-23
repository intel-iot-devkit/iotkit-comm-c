/*
 * zmq-reqrep.h
 *
 *  Created on: 2014-04-23
 *      Author: skothurx
 */

#ifndef ZMQ_REQREP_H_
#define ZMQ_REQREP_H_

//values for type --> open, ssl
int createClient(char *host, int port, char *type, void *sslargs);
int request(char *message);
int response(void (*callback)(char *message));
int close();

#endif /* ZMQ_REQREP_H_ */
