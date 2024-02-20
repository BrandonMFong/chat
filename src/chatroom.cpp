/**
 * author: brando
 * date: 2/20/24
 */

#include "chatroom.hpp"
#include <string.h>

Chatroom::Chatroom() : Object() {
	this->updateConversation = false;
}

Chatroom::~Chatroom() {

}

int Chatroom::addMessage(const Message * msg) {
	Message * m = MESSAGE_ALLOC;
	memcpy(m, msg, sizeof(Message));
	this->conversation.get().add(m);

	this->updateConversation = true;
	return 0;
}

