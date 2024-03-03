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
#include "message.hpp"
#include "chatroom.hpp"
#include "chatroomclient.hpp"
#include "chatroomserver.hpp"
#include "user.hpp"
#include "interface.hpp"
#include "packet.hpp"
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<List<Agent *>> agents;

void _AgentReleaseAgent(Agent * a) {
	BFRelease(a);
}

Agent::Agent() {
	this->_sc = NULL;
	this->_remoteuser = NULL;
}

Agent::~Agent() {
	this->_sc = NULL; // we don't own memory
	this->_remoteuser; // we do not own memory
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

		agents.lock();

		// making sure that we do not declare more than
		// 1 AgentClient in client mode
		if (sc->mode() == SOCKET_MODE_CLIENT) {
			if (agents.unsafeget().count() < 1) {
				AgentClient::setmain((AgentClient *) result);
			} else {
				LOG_ERROR("cannot have more than one AgentClient");
				BFRelease(result);
				result = NULL;
			}
		}
		
		// if agents are 0, then we can safely assume
		// the callback hasn't been set yet
		if (result) {
			if (agents.unsafeget().count() == 0) {
				agents.unsafeget().setDeallocateCallback(_AgentReleaseAgent);
			}

			BFRetain(result);
			agents.unsafeget().add(result);
		}

		agents.unlock();
	}

	return result;
}

Agent * Agent::getAgentForConnection(SocketConnection * sc) {
	if (!sc) return NULL;

	agents.lock();

	uuid_t uuid;
	sc->getuuid(uuid);

	Agent * result = NULL;
	List<Agent *>::Node * n = agents.unsafeget().first();
	for (; n; n = n->next()) {
		result = n->object();
		uuid_t tuuid;
		result->_sc->getuuid(tuuid);
		if (!uuid_compare(tuuid, uuid))
			break;
	}

	agents.unlock();

	return result;
}

// handles incoming messages
void Agent::receivedPayloadTypeMessage(const Packet * pkt) {
	// chatroom will own this memory
	Message * m = new Message(pkt);
	if (!m) {
		LOG_DEBUG("couldn't create message object");
		return;
	}

	uuid_t uuid;
	m->getuuidchatroom(uuid);
	Chatroom * chatroom = Chatroom::getChatroom(uuid);
	if (!chatroom) {
		LOG_DEBUG("chatroom not available");
		return;
	}

	int err = chatroom->addMessage(m);
	if (err) {
		LOG_DEBUG("error adding message to chatroom: %d", err);
		return;
	}
}

void Agent::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	LOG_DEBUG("our user info is being requested");

	Packet p;
	memset(&p, 0, sizeof(p));
	p.header.time = BFTimeGetCurrentTime();
	p.header.type = kPayloadTypeUserInfo;

	// Ask current user to give their information
	const User * curruser = Interface::GetCurrentUser();
	curruser->getuserinfo(&p.payload.userinfo);

	// send the information back
	this->sendPacket(&p);
}

void Agent::receivedPayloadTypeUserInfo(const Packet * pkt) {
	if (!pkt)
		return;
	LOG_DEBUG("received user info");

	// save user and save a record of the user
	this->_remoteuser = User::create(&pkt->payload.userinfo);

#if DEBUG
	uuid_t uuid;
	this->_remoteuser->getuuid(uuid);
	char uuidstr[UUID_STR_LEN];
	uuid_unparse_lower(uuid, uuidstr);
	LOG_DEBUG("user %s is now ready to join a chatroom",
		uuidstr
	);
#endif

	BFRelease(this->_remoteuser);
}

void Agent::receivedPayloadTypeRequestAvailableChatrooms(const Packet * pkt) {
	if (!pkt)
		return;
	
#ifdef DEBUG
	LOG_DEBUG("list of chatrooms are being requested");
	char uuidstr[UUID_STR_LEN];
	uuid_unparse(pkt->payload.userinfo.useruuid, uuidstr);
	LOG_DEBUG("chatroom list requested by user: %s", uuidstr);

	// maybe we can user the user info to see user permissions
	//
	// nothing is being done with it right now but just passing
	// it for now
#endif

	// gather list of chatrooms
	int size = 0;
	int error = 0;
	PayloadChatInfo ** info = Chatroom::getChatroomList(
		&size, &error
	);

	// send a packet for each chatroom
	for (int i = 0; i < size; i++) {
		// make packet
		Packet p;
		memset(&p, 0, sizeof(p));
		PacketSetHeader(&p, kPayloadTypeChatInfo);
		PacketSetPayload(&p, info[i]);

		// send packet
		this->sendPacket(&p);

		// free info
		BFFree(info[i]);
	}

	BFFree(info);
}

void Agent::receivedPayloadTypeChatroomInfo(const Packet * pkt) {
	if (!pkt)
		return;
	
	LOG_DEBUG("received chatroom information from server");
	ChatroomClient::recordChatroom(&pkt->payload.chatinfo, (AgentClient *) this);
}

void Agent::receivedPayloadTypeChatroomEnrollment(const Packet * pkt) {
	if (!pkt)
		return;

	LOG_DEBUG("chatroom enrollment");	
	ChatroomServer * chatroom = (ChatroomServer *) Chatroom::getChatroom(
		pkt->payload.chatinfo.chatroomuuid
	);

	if (!chatroom)
		return;

	BFRetain(chatroom);



	BFRelease(chatroom);
}

void Agent::packetReceive(SocketConnection * sc, const void * buf, size_t size) {
	LOG_DEBUG("> %s", __func__);
	if (!sc || !buf) 
		return;

	const Packet * p = (const Packet *) buf;
	Agent * agent = Agent::getAgentForConnection(sc);
	if (!agent)
		return;

	LOG_DEBUG("received packet: %d", p->header.type);

	switch (p->header.type) {
	case kPayloadTypeMessage:
		agent->receivedPayloadTypeMessage(p);
		break;
	case kPayloadTypeRequestUserInfo:
		agent->receivedPayloadTypeRequestInfo(p);
		break;
	case kPayloadTypeUserInfo:
		agent->receivedPayloadTypeUserInfo(p);
		break;
	case kPayloadTypeRequestChatroomList:
		agent->receivedPayloadTypeRequestAvailableChatrooms(p);
		break;
	case kPayloadTypeChatInfo:
		agent->receivedPayloadTypeChatroomInfo(p);
		break;
	case kPayloadTypeChatroomEnrollment:
		agent->receivedPayloadTypeChatroomEnrollment(p);
		break;
	}
	LOG_DEBUG("< %s", __func__);
}

int Agent::sendPacket(const Packet * pkt) {
	return this->_sc->queueData(pkt, sizeof(Packet));
}

int Agent::newConnection(SocketConnection * sc) {
	Agent * a = Agent::create(sc);

	if (!a) {
		return 1;
	} else {
		a->start();

		BFRelease(a);
		return 0;
	}
}

bool Agent::connectionIsReady() {
	return this->_sc->isready();
}

