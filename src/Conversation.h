/*
 * Conversation.h
 *
 *  Created on: Sep 28, 2009
 *      Author: psepulv
 */

#ifndef CONVERSATION_H_
#define CONVERSATION_H_

#include "Message.h"
#include <vector>

class Conversation {
protected:
	std::vector<Message*> lista;
public:
	Conversation();
	virtual ~Conversation();
	void append(Message *m);
};

#endif /* CONVERSATION_H_ */
