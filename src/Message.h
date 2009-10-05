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
	bool isReady();
	virtual ~Message();

	int getClient_ip() const
    {
        return client_ip;
    }

    short getClient_port() const
    {
        return client_port;
    }

    int getServer_ip() const
    {
        return server_ip;
    }

    short getServer_port() const
    {
        return server_port;
    }

protected:
	int server_ip;
	short server_port;
	int client_ip;
	short client_port;


};

#endif /* MESSAGE_H_ */
