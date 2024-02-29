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
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<List<Agent *>> agents;

void _AgentReleaseAgent(Agent * a) {
	BFRelease(a);
}

Agent::Agent() {
	this->_sc = NULL;
}

Agent::~Agent() {
	this->_sc = NULL; // we don't own memory
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

// handles incoming messages
void _OfficeReceivedPayloadTypeMessage(SocketConnection * sc, const Packet * p) {
	// chatroom will own this memory
	Message * m = new Message(p);
	if (!m) {
		LOG_DEBUG("couldn't create message object");
		return;
	}

	Chatroom * chatroom = ChatDirectoryGetChatroom(m->chatuuid());
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

void _OfficeReceivedPayloadTypeRequestInfo(SocketConnection * sc, const Packet * inpkt) {
	LOG_DEBUG("our user info is being requested");

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeUserInfo;

	//p.payload.userinfo
}

void Agent::packetReceive(SocketConnection * sc, const void * buf, size_t size) {
	const Packet * p = (const Packet *) buf;
	if (!p) {
		LOG_DEBUG("data is null");
		return;
	}

	LOG_DEBUG("received packet");

	switch (p->header.type) {
	case kPayloadTypeMessage:
		_OfficeReceivedPayloadTypeMessage(sc, p);
		break;
	case kPayloadTypeRequestInfo:
		_OfficeReceivedPayloadTypeRequestInfo(sc, p);
		break;
	}
}

int Agent::newConnection(SocketConnection * sc) {
	Agent * a = Agent::create(sc);
	a->start();
	return 0;
}

