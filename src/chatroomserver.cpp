/**
 * author: brando
 * date: 3/1/24
 */

#include "chatroomserver.hpp"
#include "chat.hpp"
#include "log.hpp"
#include "agentserver.hpp"
#include "user.hpp"
#include "packet.hpp"
#include "interface.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "ciphersymmetric.hpp"

using namespace BF;

ChatroomServer::ChatroomServer() : Chatroom() {
	//this->_cipher = NULL;
}

ChatroomServer::~ChatroomServer() {
	//BFRelease(this->_cipher);
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
		
		// if the agent represents the user we received our packet from,
		// skip
		if (!a->representsUserWithUUID(pkt->payload.message.useruuid)) {
			a->sendPacket(pkt);
		}
	}
	this->_agents.unlock();

	return 0;
}

int ChatroomServer::requestEnrollment(User * user) {
	Packet p;
	int error = this->fillOutEnrollmentFormRequest(user, &p);
	if (error) return error;

	if (this->fillOutEnrollmentFormResponse(&p.payload.enrollform)) {
		LOG_DEBUG("for some reason couldn't approve form");
		return 1;
	}

	return this->finalizeEnrollment(&p.payload.enrollform);
}

