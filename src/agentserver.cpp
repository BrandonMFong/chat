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
	BFThreadAsyncID tid = BFThreadAsync(AgentServer::handshake, this);
	BFThreadAsyncDestroy(tid);

	return 0;
}

void AgentServer::handshake(void * in) {
	AgentServer * a = (AgentServer *) in;
	
	LOG_DEBUG("waiting for connection");

	// wait for the connection to be ready before we
	// start the conversation with the remote user
	while (!a->_sc->isready()) {}

	LOG_DEBUG("connection is ready");

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeRequestInfo;
	a->_sc->queueData(&p, sizeof(p));
}

