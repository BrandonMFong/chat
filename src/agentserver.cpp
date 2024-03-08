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
	// need to start the converstaion with the new 
	// client
	BFThreadAsyncID tid = BFThreadAsync(AgentServer::handshake, this);
	BFThreadAsyncDestroy(tid);

	return 0;
}

void AgentServer::handshake(void * in) {
	AgentServer * a = (AgentServer *) in;
	
	// wait for the connection to be ready before we
	// start the conversation with the remote user
	while (!a->connectionIsReady()) {}

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeRequestUserInfo;
	a->sendPacket(&p);
}

void AgentServer::receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt) {
	LOG_DEBUG("this should not be invoked on servers");
}

