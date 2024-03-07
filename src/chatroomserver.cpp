/**
 * author: brando
 * date: 3/1/24
 */

#include "chatroomserver.hpp"
#include "log.hpp"
#include "agentserver.hpp"
#include "user.hpp"
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

void _AgentServerRelease(AgentServer * a) {
	BFRelease(a);
}

ChatroomServer::ChatroomServer() : Chatroom() {
	// we do not own these agents.  Their memory is management
	// within its class
	this->_agents.get().setDeallocateCallback(_AgentServerRelease);
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

void ChatroomServer::addAgent(AgentServer * a) {
	// make sure we retain this object
	BFRetain(a);

	this->_agents.get().add(a);
}

int ChatroomServer::sendPacket(const Packet * pkt) {
	this->_agents.lock();
	List<AgentServer *>::Node * n = this->_agents.unsafeget().first();
	for (; n; n = n->next()) {
		AgentServer * a = n->object();
		a->sendPacket(pkt);
	}
	this->_agents.unlock();

	return 0;
}

int ChatroomServer::receiveMessagePacket(const Packet * pkt) {
	// this adds new message to our conversation
	this->Chatroom::receiveMessagePacket(pkt);

	// now sending packet to our agents
	this->_agents.lock();
	List<AgentServer *>::Node * n = this->_agents.unsafeget().first();
	for (; n; n = n->next()) {
		AgentServer * a = n->object();
		
		uuid_t uuid;
		a->user()->getuuid(uuid);

		// if the agent represents the user we received our packet from,
		// skip
		if (uuid_compare(uuid, pkt->payload.message.useruuid)) {
			a->sendPacket(pkt);
		}
	}
	this->_agents.unlock();

	return 0;
}

