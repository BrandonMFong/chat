/**
 * author: brando
 * date: 2/20/24
 */

#include "chatdirectory.hpp"

ChatDirectory * _sharedChatDir = NULL;

ChatDirectory * ChatDirectory::shared() {
	return _sharedChatDir;
}

ChatDirectory::ChatDirectory() : Object() {
	_sharedChatDir = this;
}

ChatDirectory::~ChatDirectory() {

}

Chatroom * ChatDirectory::getChatroom() {
	return this->_chatrooms.get().first()->object();
}

void ChatDirectory::addChatroom(Chatroom * room) {
	this->_chatrooms.get().add(room);
}

