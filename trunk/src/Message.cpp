/*
 * Message.cpp
 *
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 */

#include "Message.h"

Message::Message() {
	// TODO Auto-generated constructor stub

}

bool Message::isReady()
{
	return true;
}

Message::Message(int ip_server, short  port_server, int ip_client, short  port_client)
{
}

void Message::append(char msg[], size_t l)
{
}


Message::~Message() {
	// TODO Auto-generated destructor stub
}
