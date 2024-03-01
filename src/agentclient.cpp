/**
 * author: brando
 * date: 2/27/24
 */

#include "agentclient.hpp"
#include "log.hpp"
#include "connection.hpp"
#include <string.h>

AgentClient::AgentClient() : Agent() {

}

AgentClient::~AgentClient() {

}

int AgentClient::start() {
	return 0;
}

void AgentClient::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	this->Agent::receivedPayloadTypeRequestInfo(pkt);
	LOG_DEBUG("we will now request the server for their information");

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeRequestUserInfo;
	this->_sc->queueData(&p, sizeof(p));
}

