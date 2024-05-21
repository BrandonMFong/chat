/**
 * author: brando
 * date: 2/27/24
 */

#include "agentclient.hpp"
#include "log.hpp"
#include "packet.hpp"
#include "interface.hpp"
#include "chatroom.hpp"
#include "user.hpp"
#include <string.h>
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include <bfnet/bfnet.hpp>

using namespace BF;
using namespace BF::Net;

Atomic<AgentClient *> agentclientmain;

void _AgentClientReleaseUser(User * user) {
	BFRelease(user);
}

AgentClient::AgentClient() : Agent() {
	this->_remoteusers.get().setReleaseCallback(_AgentClientReleaseUser);
}

AgentClient::~AgentClient() {

}

User * AgentClient::getremoteuser(uuid_t uuid) {
	User * result = NULL;
	this->_remoteusers.lock();
	List<User *>::Node * n = this->_remoteusers.unsafeget().first();
	for(; n; n = n->next()) {
		User * user = n->object();
		uuid_t u;
		user->getuuid(u);
		if (!uuid_compare(u, uuid)) {
			result = user;
			break;
		}
	}
	this->_remoteusers.unlock();

	return result;
}

void AgentClient::setremoteuser(User * user) {
	this->_remoteusers.lock();
	if (!this->_remoteusers.unsafeget().contains(user)) {
		this->_remoteusers.unsafeget().add(user);
	}
	this->_remoteusers.unlock();
}

int AgentClient::start() {
	return 0;
}

void AgentClient::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	// we will send information back to sender
	this->Agent::receivedPayloadTypeRequestInfo(pkt);

	// we will also require them to send us information about them
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeRequestUserInfo);
	this->sendPacket(&p);
}

AgentClient * AgentClient::getmain() {
	while (!agentclientmain.get()) { usleep(50); } // make sure its available
	return agentclientmain.get();
}

void AgentClient::setmain(AgentClient * ac) {
	agentclientmain.set(ac);
}

int AgentClient::requestChatroomListUpdate(const User * user) {
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeRequestChatroomList);
	Interface::current()->getuser()->getuserinfo(&p.payload.userinfo);
	return this->sendPacket(&p);
}

void AgentClient::receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt) {
	if (!pkt)
		return;

	LOG_DEBUG("received a notification that chatroom list has changed");

	// clear our chatrooms
	Chatroom::clearChatroomList();

	// ask server to send updated list of chatrooms
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeRequestChatroomList);
	Interface::current()->getuser()->getuserinfo(&p.payload.userinfo);
	this->sendPacket(&p);
}

void AgentClient::receivedPayloadTypeNotifyQuitApp(const Packet * pkt) {
	this->_remoteusers.lock();

	User * user = User::getuser(pkt->payload.userinfo.useruuid);
	this->_remoteusers.unsafeget().pluckObject(user);
	BFRelease(user);
	
	User::destroy(user);

	this->_remoteusers.unlock();
}

bool AgentClient::representsUserWithUUID(const uuid_t uuid) {
	bool result = false;
	this->_remoteusers.lock();
	List<User *>::Node * n = this->_remoteusers.unsafeget().first();
	for(; n; n = n->next()) {
		User * user = n->object();
		uuid_t u;
		user->getuuid(u);
		if (!uuid_compare(u, uuid)) {
			result = true;
			break;
		}
	}
	this->_remoteusers.unlock();

	return result;
}

void AgentClient::updateremoteuser(const PayloadUserInfo * info) {
// TODO: update user
}

