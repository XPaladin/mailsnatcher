/*
 * MessageManager.cpp
 *
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 */

#include "MessageManager.h"
#include "PopMessage.h"
#include <sstream>

MessageManager::MessageManager() {
    //  messageTable= new map<string,Message>();
}
MessageManager::~MessageManager(){

}

void MessageManager::insert(int client_ip, short client_port,
		int server_ip, short server_port,
		const unsigned char message[], size_t largo){
	Message *m;
	std::string header= headerString(client_ip, client_port,
			server_ip, server_port);

	std::map<std::string,Message*>::iterator iter=messageTable.find(header);

	if( iter == messageTable.end() ){
		switch(server_port){
		case 110:
			m=new PopMessage(client_ip, client_port,
				server_ip, server_port);
			break;
		default: 
                        /* ¡No se puede instanciar una clase virtual! */
                        return;
                }
	}else{
		m = iter->second;
	}
	m->append(message, largo);
	messageTable[header] = m;

}

Message* MessageManager::remove(int client_ip, short client_port,
		int server_ip, short server_port){
	std::string header= headerString(client_ip, client_port,
			server_ip, server_port);

	std::map<std::string,Message*>::iterator iter=messageTable.find(header);
	messageTable.erase(header);
	return iter == messageTable.end()? NULL : iter->second;

}

Message* MessageManager::get(int client_ip, short client_port,
		int server_ip, short server_port){
	std::string header= headerString(client_ip, client_port,
			server_ip, server_port);

	std::map<std::string,Message*>::iterator iter=messageTable.find(header);
	return iter == messageTable.end()? NULL : iter->second;

}

std::string MessageManager::headerString(
                            int client_ip, short client_port,
                            int server_ip, short server_port){
        std::ostringstream buffer;
        buffer << client_ip << client_port << server_ip << server_port;
        return buffer.str();
}



