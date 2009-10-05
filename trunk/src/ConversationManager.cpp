/*
 * ConversationManager.cpp
 *
 *  Created on: Sep 28, 2009
 *      Author: psepulv
 */

#include "ConversationManager.h"
#include "MessageManager.h"


ConversationManager::ConversationManager() {
	// TODO Auto-generated constructor stub

}

ConversationManager::~ConversationManager() {
	// TODO Auto-generated destructor stub
}
void ConversationManager::insert(Message *m){
	std::string header= MessageManager::headerString(m->getClient_ip(), m->getClient_port(),
			m->getServer_ip(), m->getServer_port());

	std::map<std::string,Conversation*>::iterator iter=conversationTable.find(header);
	Conversation *c;
	if( iter == conversationTable.end() ){
		c = new Conversation();
		}else{
			c = iter->second;
		}
		c->append(m);
	 conversationTable[header] = c;
}
