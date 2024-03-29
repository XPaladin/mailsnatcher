/*
 * MessageManager.cpp
 *
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 */

#include "MessageManager.h"
#include "PopMessage.h"
#include "SmtpMessage.h"
#include "HttpMessage.h"
#include <sstream>
#include <stdio.h>
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

	//printf("in :%s\n",header.c_str());
	std::map<std::string,Message*>::iterator iter=messageTable.find(header);

	if( iter == messageTable.end() ){
		switch(server_port){
		case 25:
//			fprintf(stderr,"pop:%d\n",server_port);
			m=new SmtpMessage(server_ip, server_port,
				client_ip, client_port);
			break;
		case 110:
//			fprintf(stderr,"pop:%d\n",server_port);
			m=new PopMessage(server_ip, server_port,
				client_ip, client_port);
			break;
		case 80:
//			fprintf(stderr,"Http:%d\n",server_port);
			m=new HttpMessage(server_ip, server_port,
				client_ip, client_port);
			break;
		default: 
                        /* ¡No se puede instanciar una clase virtual! */
			fprintf(stderr,"Protocolo no entendido:%d\n",server_port);
                        return;
                }
	}else{
		m = iter->second;
	}
	m->append(message, largo);
	messageTable[header] = m;
	//printf("in :%s\n",header.c_str());
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
	//printf("out:%s\n",header.c_str());
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



