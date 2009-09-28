/*
 * MessageManager.h
 *
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 */

#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include <map>
#include <string>
#include "Message.h"


class MessageManager{
protected:
    std::map<std::string, Message*> messageTable;

public:
    MessageManager();

    void insert(int client_ip, short client_port,
                int server_ip, short server_port,
                const unsigned char message[], size_t largo);

    Message* remove(int client_ip, short client_port,
                    int server_ip, short server_port);

    static std::string headerString(int client_ip, short client_port,
                            int server_ip, short server_port);

	virtual ~MessageManager();
};

#endif /* MESSAGEMANAGER_H_ */
