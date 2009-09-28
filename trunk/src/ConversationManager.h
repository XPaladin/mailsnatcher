/*
 * ConversationManager.h
 *
 *  Created on: Sep 28, 2009
 *      Author: psepulv
 */

#ifndef CONVERSATIONMANAGER_H_
#define CONVERSATIONMANAGER_H_

#include "Conversation.h"
#include <map>

class ConversationManager {
protected:
    std::map<std::string, Conversation*> conversationTable;

public:
    void insert(Message *m);
    ConversationManager();
	virtual ~ConversationManager();
};

#endif /* CONVERSATIONMANAGER_H_ */
