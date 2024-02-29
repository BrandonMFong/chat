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

