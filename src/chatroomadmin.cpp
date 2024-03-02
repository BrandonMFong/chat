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
	LOG_DEBUG("creating new chatroom");
	ChatroomAdmin * cr = new ChatroomAdmin;
	strncpy(cr->_name, name, sizeof(cr->_name));

	Chatroom::addRoomToChatrooms(cr);
	return cr;
}

