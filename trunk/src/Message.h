/*
 * Message.h
 *
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>

class Message {
public:
	Message();
	Message(int ip_server, short port_server,
	        int ip_client,short port_client);

	void append(char msg[], size_t l);
	int server_ip;
	short server_port;
	int client_ip;
	short client_port;

	virtual ~Message();
};

#endif /* MESSAGE_H_ */
