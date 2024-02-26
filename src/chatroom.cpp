/**
 * author: brando
 * date: 2/20/24
 */

#include "chatroom.hpp"
#include "office.hpp"
#include "user.hpp"
#include "log.hpp"
#include "message.hpp"
#include <string.h>
#include <bflibcpp/delete.hpp>

void _ChatroomMessageFree(Message * m) {
	Delete(m);
}

Chatroom::Chatroom(const char * uuid) : Object() {
	this->updateConversation = false;
	BFStringGetRandomUUIDString(this->_uuid);
	memcpy(this->_uuid, uuid, sizeof(this->_uuid));
	memset(this->_name, 0, sizeof(this->_name));

	// setup conversation thread
	this->conversation.get().setDeallocateCallback(_ChatroomMessageFree);
}

Chatroom::~Chatroom() {

}

int Chatroom::addMessage(Message * msg) {
	if (!msg) return 2;
	
	this->conversation.get().add(msg);
	this->updateConversation = true;

	return 0;
}

void Chatroom::setName(const char * name) {
	strncpy(this->_name, name, sizeof(this->_name));
}

const char * Chatroom::uuid() {
	return this->_uuid;
}

int Chatroom::sendBuffer(const InputBuffer * buf) {
	Packet p;

	memset(&p, 0, sizeof(p));

	// load buffer 
	strncpy(p.payload.message.data, buf->cString(), sizeof(p.payload.message.data));

	// username
	strncpy(p.payload.message.username, User::current()->username(), sizeof(p.payload.message.username));
	
	// user uuid
	strncpy(p.payload.message.useruuid, User::current()->uuid(), sizeof(p.payload.message.useruuid));

	// chatroom uuid
	strncpy(p.payload.message.chatuuid, this->_uuid, kBFStringUUIDStringLength);

	// time
	p.header.time = BFTimeGetCurrentTime();

	// set payload type
	p.header.type = kPayloadTypeMessage;

	// give chatroom this message to add to 
	// its list
	this->addMessage(new Message(&p));

	// send out message to the rest of the users in the 
	// chatroom
	return Office::PacketSend(&p);
}

