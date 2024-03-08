/**
 * author: brando
 * date: 2/27/24
 */

#include "agentclient.hpp"
#include "log.hpp"
#include "connection.hpp"
#include "packet.hpp"
#include "interface.hpp"
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
	LOG_DEBUG("we will now request the server for their information");

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
	LOG_DEBUG("pinging server to give us an up to date list of chatrooms we can join");
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeRequestChatroomList);
	Interface::current()->getuser()->getuserinfo(&p.payload.userinfo);
	return this->sendPacket(&p);
}

/*
int AgentClient::enrollInChatroom(const PayloadChatInfo * chatinfo) {
	LOG_DEBUG("enrolling in chatroom");
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeChatroomEnrollment);
	PacketSetPayload(&p, chatinfo);
	return this->sendPacket(&p);
}
*/

