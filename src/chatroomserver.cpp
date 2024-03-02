/**
 * author: brando
 * date: 3/1/24
 */

#include "chatroomserver.hpp"
#include "log.hpp"

ChatroomServer::ChatroomServer() : Chatroom() {

}

ChatroomServer::~ChatroomServer() {

}

ChatroomServer * ChatroomServer::create(const char * name) {
	ChatroomServer * cr = new ChatroomServer;
	strncpy(cr->_name, name, sizeof(cr->_name));

#ifdef DEBUG
	char uuidstr[UUID_STR_LEN];
	uuid_unparse(cr->_uuid, uuidstr);
	LOG_DEBUG("creating new chatroom: %s - %s", cr->_name, uuidstr);
#endif

	Chatroom::addRoomToChatrooms(cr);
	return cr;
}

