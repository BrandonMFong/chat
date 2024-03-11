/**
 * author: brando
 * date: 3/2/24
 */

#include "chatroomclient.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"
#include "interface.hpp"
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
	if (!info || !agent)
		return 1;

	ChatroomClient * chatroom = new ChatroomClient(agent, info->chatroomuuid);
	if (!chatroom)
		return 2;

	memcpy(chatroom->_name, info->chatroomname, sizeof(chatroom->_name));

	Chatroom::addRoomToChatrooms(chatroom);

	// tell the ui that our chatroom list
	// changed so they can update the ui
	Interface::current()->chatroomListHasChanged();

	BFRelease(chatroom);

	return 0;
}

int ChatroomClient::sendPacket(const Packet * pkt) {
	return this->agent()->sendPacket(pkt);
}

