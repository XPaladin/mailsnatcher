/*
 * MessageManager.cpp
 *
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 */

#include "MessageManager.h"
#include <sstream>
MessageManager::MessageManager(){
    //  messageTable= new map<string,Message>();
}

void MessageManager::insert(int client_ip, short client_port,
		int server_ip, short server_port, std::string message){
	Message *m;
	std::string header= headerString(client_ip, client_port,
			server_ip, server_port);

	std::map<std::string,Message*>::iterator iter=messageTable.find(header);

	if( iter == messageTable.end() ){
		m = new Message(client_ip, client_port,
				server_ip, server_port);
	}else{
		m = iter->second;
	}
	m->append(message);
	messageTable[header] = m;

}


std::string MessageManager::headerString(
                            int client_ip, short client_port,
                            int server_ip, short server_port){
        std::ostringstream buffer;
        buffer << client_ip << client_port << server_ip << server_port;
        return buffer.str();
}



