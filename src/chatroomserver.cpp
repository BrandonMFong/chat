/**
 * author: brando
 * date: 3/1/24
 */

#include "chatroomserver.hpp"
#include "log.hpp"
#include "agentserver.hpp"
#include "user.hpp"
#include "packet.hpp"
#include "interface.hpp"
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

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

	// tell the ui that our chatroom list
	// changed so they can update the ui
	Interface::current()->chatroomListHasChanged();

	// broadcast to all clients that chatroom list changed
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeNotifyChatroomListChanged);

	Agent::broadcast(&p);

	return cr;
}

int ChatroomServer::sendPacket(const Packet * pkt) {
	this->_agents.lock();
	List<Agent *>::Node * n = this->_agents.unsafeget().first();
	for (; n; n = n->next()) {
		Agent * a = n->object();
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
	List<Agent *>::Node * n = this->_agents.unsafeget().first();
	for (; n; n = n->next()) {
		Agent * a = n->object();
		
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

