/**
 * author: brando
 * date: 3/2/24
 */

#include "chatroomclient.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"
#include "agentclient.hpp"

ChatroomClient::ChatroomClient(AgentClient * a, const uuid_t chatroomuuid) : Chatroom(chatroomuuid) {
	this->addAgent(a);
}

ChatroomClient::~ChatroomClient() {
}

AgentClient * ChatroomClient::agent() {
	return (AgentClient *) this->_agents.get().first()->object();
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
	return this->agent()->sendPacket(pkt);
}

