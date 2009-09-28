/*
 * Conversation.cpp
 *
 *  Created on: Sep 28, 2009
 *      Author: psepulv
 */

#include "Conversation.h"

Conversation::Conversation() {

}

Conversation::~Conversation() {

}

void Conversation::append(Message *m)
{
	lista.push_back(m);
}


