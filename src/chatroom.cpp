/**
 * author: brando
 * date: 2/20/24
 */

#include "chatroom.hpp"
#include <string.h>

Chatroom::Chatroom() : Object() {
	this->updateConversation = false;
	BFStringGetRandomUUIDString(this->_uuid);
	memset(this->_name, 0, sizeof(this->_name));
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

void Chatroom::setName(const char * name) {
	strncpy(this->_name, name, sizeof(this->_name));
}

