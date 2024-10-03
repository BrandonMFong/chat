/**
 * author: brando
 * date: 3/2/24
 */

#include "chatroomclient.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"
#include "interface.hpp"
#include "agentclient.hpp"
#include "packet.hpp"
#include "user.hpp"

using namespace BF;

ChatroomClient::ChatroomClient(const uuid_t chatroomuuid, AgentClient * agent) : Chatroom(chatroomuuid) {
	this->addAgent(agent);
}

ChatroomClient::~ChatroomClient() {
}

AgentClient * ChatroomClient::agent() {
	return (AgentClient *) this->_agents.get().first()->object();
}

int ChatroomClient::recordChatroom(const PayloadChatInfo * info, Agent * agent) {
	if (!info || !agent)
		return 1;

	ChatroomClient * chatroom = new ChatroomClient(info->chatroomuuid, (AgentClient *) agent);
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

int ChatroomClient::requestEnrollment(User * user) {
	Packet p;
	int error = this->fillOutEnrollmentFormRequest(user, &p);
	if (error) return error;

	LOG_DEBUG("sending enrollment form: %d", kPayloadTypeChatroomEnrollmentForm);
	return this->sendPacket(&p);
}

