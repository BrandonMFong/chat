/**
 * author: brando
 * date: 2/20/24
 */

#include "chatroom.hpp"
#include "office.hpp"
#include "user.hpp"
#include <string.h>

Chatroom::Chatroom() : Object() {
	this->updateConversation = false;
	BFStringGetRandomUUIDString(this->_uuid);
	strncpy(this->_uuid, "ea46019c-4c39-4838-b44d-6a990bbb4ae9", kBFStringUUIDStringLength);
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

int Chatroom::sendBuffer(const InputBuffer * buf) {
	Message m;

	// clear packet
	memset(&m, 0, sizeof(m));
	// load buffer 
	strncpy(m.buf, buf->cString(), sizeof(m.buf));

	// username
	strncpy(m.username, User::current()->username(), sizeof(m.username));

	// time
	m.time = BFTimeGetCurrentTime();

	// give chatroom this message to add to 
	// its list
	this->addMessage(&m);

	// send to socket
	return Office::MessageSend(&m);
}

