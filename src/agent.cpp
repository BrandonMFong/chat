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
	}

	return result;
}

int Agent::start() {
	// if we are on the server, then we
	// need to start the converstaion
	if (this->_sc->mode() == SOCKET_MODE_SERVER) {
		BFThreadAsyncID tid = BFThreadAsync(Agent::handshake, this);
		BFThreadAsyncDestroy(tid);
	} else {

	}

	return 0;
}

void Agent::handshake(void * in) {
	Agent * a = (Agent *) in;
	
	LOG_DEBUG("waiting for connection");

	// wait for the connection to be ready before we
	// start the conversation with the remote user
	while (!a->_sc->isready()) {}

	LOG_DEBUG("connection is ready");
}

