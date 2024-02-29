/**
 * author: brando
 * date: 2/26/24
 */

#include "agent.hpp"
#include "agentserver.hpp"
#include "agentclient.hpp"
#include "log.hpp"
#include "socket.hpp"
#include "connection.hpp"
#include "message.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"
#include "user.hpp"
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<List<Agent *>> agents;

void _AgentReleaseAgent(Agent * a) {
	BFRelease(a);
}

Agent::Agent() {
	this->_sc = NULL;
	this->_remoteuser = NULL;
}

Agent::~Agent() {
	this->_sc = NULL; // we don't own memory
	this->_remoteuser; // we do not own memory
}

Agent * Agent::create(SocketConnection * sc) {
	if (!sc) return NULL;

	Agent * result = NULL;
	switch (sc->mode()) {
	case SOCKET_MODE_SERVER:
		result = new AgentServer;
		break;
	case SOCKET_MODE_CLIENT:
		result = new AgentClient;
		break;
	default:
		break;
	}

	if (result) {
		result->_sc = sc;

		agents.lock();

		// if agents are 0, then we can safely assume
		// the callback hasn't been set yet
		if (agents.unsafeget().count() == 0) {
			agents.unsafeget().setDeallocateCallback(_AgentReleaseAgent);
		}

		agents.unsafeget().add(result);
		agents.unlock();
	}

	return result;
}

Agent * Agent::getAgentForConnection(SocketConnection * sc) {
	if (!sc) return NULL;

	agents.lock();

	uuid_t uuid;
	sc->getuuid(uuid);

	Agent * result = NULL;
	List<Agent *>::Node * n = agents.unsafeget().first();
	for (; n; n = n->next()) {
		result = n->object();
		uuid_t tuuid;
		result->_sc->getuuid(tuuid);
		if (!uuid_compare(tuuid, uuid))
			break;
	}

	agents.unlock();

	return result;
}

// handles incoming messages
void Agent::receivedPayloadTypeMessage(const Packet * pkt) {
	// chatroom will own this memory
	Message * m = new Message(pkt);
	if (!m) {
		LOG_DEBUG("couldn't create message object");
		return;
	}

	uuid_t uuid;
	m->getuuidchatroom(uuid);
	Chatroom * chatroom = ChatDirectoryGetChatroom(uuid);
	if (!chatroom) {
		LOG_DEBUG("chatroom not available");
		return;
	}

	int err = chatroom->addMessage(m);
	if (err) {
		LOG_DEBUG("error adding message to chatroom: %d", err);
		return;
	}
}

void Agent::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	LOG_DEBUG("our user info is being requested");

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeUserInfo;

	// Ask current user to give their information
	const User * curruser = User::current();
	curruser->getuserinfo(&p.payload.userinfo);

	// send the information back
	this->_sc->queueData(&p, sizeof(p));
}

void Agent::receivedPayloadTypeUserInfo(const Packet * pkt) {
	if (!pkt)
		return;
	LOG_DEBUG("received user info");

	// save user and save a record of the user
	this->_remoteuser = User::create(&pkt->payload.userinfo);
}

void Agent::packetReceive(SocketConnection * sc, const void * buf, size_t size) {
	LOG_DEBUG("> %s", __func__);
	if (!sc || !buf) 
		return;

	const Packet * p = (const Packet *) buf;
	Agent * agent = Agent::getAgentForConnection(sc);
	if (!agent)
		return;

	LOG_DEBUG("received packet");

	switch (p->header.type) {
	case kPayloadTypeMessage:
		agent->receivedPayloadTypeMessage(p);
		break;
	case kPayloadTypeRequestInfo:
		agent->receivedPayloadTypeRequestInfo(p);
		break;
	case kPayloadTypeUserInfo:
		agent->receivedPayloadTypeUserInfo(p);
		break;
	}
	LOG_DEBUG("< %s", __func__);
}

int Agent::newConnection(SocketConnection * sc) {
	Agent * a = Agent::create(sc);
	a->start();
	return 0;
}

