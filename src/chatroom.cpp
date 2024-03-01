/**
 * author: brando
 * date: 2/20/24
 */

#include "chatroom.hpp"
#include "office.hpp"
#include "user.hpp"
#include "log.hpp"
#include "message.hpp"
#include "interface.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<List<Chatroom *>> chatrooms;

void _ChatroomMessageFree(Message * m) {
	Delete(m);
}

void _ChatroomRelease(Chatroom * cr) {
	BFRelease(cr);
}

Chatroom::Chatroom() : Object() {
	this->updateConversation = false;
	uuid_generate_random(this->_uuid);
	memset(this->_name, 0, sizeof(this->_name));

	// setup conversation thread
	this->conversation.get().setDeallocateCallback(_ChatroomMessageFree);
}

Chatroom::~Chatroom() {

}

void _ChatroomAddRoomToChatrooms(Chatroom * cr) {
	chatrooms.lock();

	if (chatrooms.unsafeget().count() == 0) {
		chatrooms.unsafeget().setDeallocateCallback(_ChatroomRelease);
	}

	BFRetain(cr);
	chatrooms.unsafeget().add(cr);
	chatrooms.unlock();
}

Chatroom * Chatroom::create() {
	Chatroom * cr = new Chatroom;
	_ChatroomAddRoomToChatrooms(cr);
	return cr;
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

void Chatroom::getuuid(uuid_t uuid) {
	uuid_copy(uuid, this->_uuid);
}

int Chatroom::sendBuffer(const InputBuffer * buf) {
	Packet p;

	memset(&p, 0, sizeof(p));

	// load buffer 
	strncpy(p.payload.message.data, buf->cString(), sizeof(p.payload.message.data));

	// username
	strncpy(p.payload.message.username, Interface::GetCurrentUser()->username(), sizeof(p.payload.message.username));

	uuid_t uuid;
	Interface::GetCurrentUser()->getuuid(uuid);	

	// user uuid
	uuid_copy(p.payload.message.useruuid, uuid);

	// chatroom uuid
	uuid_copy(p.payload.message.chatuuid, this->_uuid);

	// time
	p.header.time = BFTimeGetCurrentTime();

	// set payload type
	p.header.type = kPayloadTypeMessage;

	// give chatroom this message to add to 
	// its list
	this->addMessage(new Message(&p));

	// send out message to the rest of the chatrooms.in the 
	// chatroom
	return Office::PacketSend(&p);
}

Chatroom * Chatroom::getChatroom(uuid_t chatroomuuid) {
	Chatroom * room = NULL;
	chatrooms.lock();
	List<Chatroom *>::Node * n = chatrooms.unsafeget().first();
	for (; n != NULL; n = n->next()) {
		Chatroom * troom = n->object();
		uuid_t uuid;
		troom->getuuid(uuid);
		if (!uuid_compare(uuid, chatroomuuid)) {
			room = troom;
			break;
		}
	}
	chatrooms.unlock();

	return room;
}

