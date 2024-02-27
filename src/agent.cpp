/**
 * author: brando
 * date: 2/26/24
 */

#include "agent.hpp"
#include <bflibcpp/bflibcpp.hpp>

Agent::Agent(SocketConnection * sc) {
	this->_sc = sc;
}

Agent::~Agent() {

}

int Agent::start() {
	BFThreadAsyncID tid = BFThreadAsync(Agent::handshake, this);
	BFThreadAsyncDestroy(tid);
	return 0;
}

void Agent::handshake(void * in) {
	Agent * a = (Agent *) in;

	// wait for the connection to be ready before we
	// start the conversation with the remote user
	while (!a->_sc->isready()) {}
}

