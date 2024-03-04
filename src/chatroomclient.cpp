/**
 * author: brando
 * date: 3/2/24
 */

#include "chatroomclient.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"
#include "agentclient.hpp"

ChatroomClient::ChatroomClient(AgentClient * a, const uuid_t chatroomuuid) : Chatroom(chatroomuuid) {
	this->_agent = a;
	BFRetain(this->_agent);
}

ChatroomClient::~ChatroomClient() {
	BFRelease(this->_agent);
}

int ChatroomClient::recordChatroom(const PayloadChatInfo * info, AgentClient * agent) {
	if (!info)
		return 1;

	ChatroomClient * chatroom = new ChatroomClient(agent, info->chatroomuuid);
	if (!chatroom)
		return 2;

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

int ChatroomClient::sendPacket(const Packet * pkt) {
	return this->_agent->sendPacket(pkt);
}

