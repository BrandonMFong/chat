/**
 * author: brando
 * date: 2/27/24
 */

#include "agentserver.hpp"
#include "log.hpp"
#include "connection.hpp"
#include "socket.hpp"
#include "user.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>

using namespace BF;

AgentServer::AgentServer() {
	this->_remoteuser = NULL;
}

AgentServer::~AgentServer() {
	BFRelease(this->_remoteuser);
}

User * AgentServer::getremoteuser(uuid_t uuid) {
	return this->_remoteuser;
}

void AgentServer::setremoteuser(User * user) {
	this->_remoteuser = user;
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
	while (!a->connectionIsReady()) { usleep(500); }

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeRequestUserInfo;
	a->sendPacket(&p);
}

int AgentServer::forwardPacket(const Packet * pkt) {
	Atomic<List<Agent *>> * agents = Agent::agentlist();
	agents->lock();

	// forward packet to the other clients
	List<Agent *>::Node * n = agents->unsafeget().first();
	for (; n; n = n->next()) {
		Agent * a = n->object();
		if (a != this) {
			a->sendPacket(pkt);
		}
	}

	agents->unlock();

	return 0;
}

void AgentServer::receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt) {
	LOG_DEBUG("this should not be invoked on servers");
}

void AgentServer::receivedPayloadTypeNotifyQuitApp(const Packet * pkt) {
	this->forwardPacket(pkt);

	Atomic<List<Agent *>> * agents = Agent::agentlist();
	agents->lock();
	// removes from list
	agents->unsafeget().pluckObject(this);

	// destroy our user
	User::destroy(this->_remoteuser);

	// releases itself
	//
	// this is safe because Agent::packetReceive is still retaining us
	Object::release(this);

	agents->unlock();
}

void AgentServer::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	// let base class send back our info
	this->Agent::receivedPayloadTypeRequestInfo(pkt);

	// send this info request to the other agents
	this->forwardPacket(pkt);
}

void AgentServer::receivedPayloadTypeUserInfo(const Packet * pkt) {
	// let base class create user 
	this->Agent::receivedPayloadTypeUserInfo(pkt);

	this->forwardPacket(pkt);
}

bool AgentServer::representsUserWithUUID(const uuid_t uuid) {
	if (!this->_remoteuser)
		return false;

	uuid_t u;
	this->_remoteuser->getuuid(u);
	if (!uuid_compare(u, uuid)) {
		return true;
	}
	
	return false;
}

void AgentServer::updateremoteuser(const PayloadUserInfo * info) {
// TODO: update user
}

