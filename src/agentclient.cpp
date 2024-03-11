/**
 * author: brando
 * date: 2/27/24
 */

#include "agentclient.hpp"
#include "log.hpp"
#include "connection.hpp"
#include "packet.hpp"
#include "interface.hpp"
#include "chatroom.hpp"
#include "user.hpp"
#include <string.h>
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<AgentClient *> agentclientmain;

AgentClient::AgentClient() : Agent() {

}

AgentClient::~AgentClient() {

}

int AgentClient::start() {
	return 0;
}

void AgentClient::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	this->Agent::receivedPayloadTypeRequestInfo(pkt);

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

	// clear our chatrooms
	Chatroom::clearChatroomList();

	// ask server to send updated list of chatrooms
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeRequestChatroomList);
	Interface::current()->getuser()->getuserinfo(&p.payload.userinfo);
	this->sendPacket(&p);
}

void AgentClient::requestPayloadTypeNotifyQuitApp(const Packet * pkt) {
// does nothing
}

