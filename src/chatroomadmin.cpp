/**
 * author: brando
 * date: 3/1/24
 */

#include "chatroomadmin.hpp"
#include "log.hpp"

ChatroomAdmin::ChatroomAdmin() : Chatroom() {

}

ChatroomAdmin::~ChatroomAdmin() {

}

ChatroomAdmin * ChatroomAdmin::create(const char * name) {
	ChatroomAdmin * cr = new ChatroomAdmin;
	strncpy(cr->_name, name, sizeof(cr->_name));

#ifdef DEBUG
	char uuidstr[UUID_STR_LEN];
	uuid_unparse(cr->_uuid, uuidstr);
	LOG_DEBUG("creating new chatroom: %s - %s", cr->_name, uuidstr);
#endif

	Chatroom::addRoomToChatrooms(cr);
	return cr;
}

