/**
 * author: brando
 * date: 2/20/24
 */

#include "chatdirectory.hpp"
#include "chatroom.hpp"
#include "log.hpp"
#include <string.h>

using namespace BF;

ChatDirectory * _sharedChatDir = NULL;

ChatDirectory * ChatDirectory::shared() {
	return _sharedChatDir;
}

ChatDirectory::ChatDirectory() : Object() {
	_sharedChatDir = this;
}

ChatDirectory::~ChatDirectory() {

}

Chatroom * ChatDirectory::getChatroom(const char * chatroomuuid) {
	Chatroom * room = NULL;
	this->_chatrooms.lock();
	List<Chatroom *>::Node * n = this->_chatrooms.unsafeget().first();
	for (; n != NULL; n = n->next()) {
		Chatroom * troom = n->object();
		LOG_DEBUG("%s ?= %s", troom->uuid(), chatroomuuid);
		if (!BFStringCompareUUID(troom->uuid(), chatroomuuid)) {
			LOG_DEBUG("found chat room: %s", troom->uuid());
			room = troom;
			break;
		}
	}
	this->_chatrooms.unlock();

	return room;
}

void ChatDirectory::addChatroom(Chatroom * room) {
	this->_chatrooms.get().add(room);
}

