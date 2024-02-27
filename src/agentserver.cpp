/**
 * author: brando
 * date: 2/27/24
 */

#include "agentserver.hpp"
#include "log.hpp"
#include "connection.hpp"
#include "socket.hpp"
#include <bflibcpp/bflibcpp.hpp>

AgentServer::AgentServer() {

}

AgentServer::~AgentServer() {

}

int AgentServer::start() {
	// if we are on the server, then we
	// need to start the converstaion
	if (this->_sc->mode() == SOCKET_MODE_SERVER) {
		BFThreadAsyncID tid = BFThreadAsync(AgentServer::handshake, this);
		BFThreadAsyncDestroy(tid);
	} else {

	}

	return 0;
}

void AgentServer::handshake(void * in) {
	AgentServer * a = (AgentServer *) in;
	
	LOG_DEBUG("waiting for connection");

	// wait for the connection to be ready before we
	// start the conversation with the remote user
	while (!a->_sc->isready()) {}

	LOG_DEBUG("connection is ready");
}

