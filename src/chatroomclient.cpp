/**
 * author: brando
 * date: 3/2/24
 */

#include "chatroomclient.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"

ChatroomClient::ChatroomClient() : Chatroom() {

}

ChatroomClient::~ChatroomClient() {

}

int ChatroomClient::recordChatroom(const PayloadChatroomInfoBrief * info) {
	if (!info)
		return 1;

	ChatroomClient * chatroom = new ChatroomClient;
	if (!chatroom)
		return 2;

	uuid_copy(chatroom->_uuid, info->chatroomuuid);
	memcpy(chatroom->_name, info->chatroomname, sizeof(chatroom->_name));

#ifdef DEBUG
	char uuidstr[UUID_STR_LEN];
	uuid_unparse(chatroom->_uuid, uuidstr);
	LOG_DEBUG("new chat room: %s - %s", chatroom->_name, uuidstr);
#endif

	Chatroom::addRoomToChatrooms(chatroom);

	BFRelease(chatroom);

	return 0;
}


