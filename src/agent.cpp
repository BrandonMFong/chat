/**
 * author: brando
 * date: 2/26/24
 */

#include "agent.hpp"
#include "agentserver.hpp"
#include "agentclient.hpp"
#include "log.hpp"
#include "message.hpp"
#include "chatroom.hpp"
#include "chatroomclient.hpp"
#include "chatroomserver.hpp"
#include "user.hpp"
#include "interface.hpp"
#include "packet.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include <bfnet/bfnet.hpp>

using namespace BF;
using namespace BF::Net;

// every agent in this list is retained with 
// rc of 1
Atomic<List<Agent *>> agents;

Atomic<List<Agent *>> * Agent::agentlist() {
	return &agents;
}

void _AgentReleaseAgent(Agent * a) {
	BFRelease(a);
}

Agent::Agent() {
	this->_sc = NULL;
}

Agent::~Agent() {
	this->_sc = NULL; // we don't own memory
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
		
		if (result) {
			// if we have agents 0, then we can safely assume
			// the callback hasn't been set yet
			if (agents.unsafeget().count() == 0) {
				agents.unsafeget().setReleaseCallback(_AgentReleaseAgent);
			}

			BFRetain(result);
			agents.unsafeget().add(result);
		}

		agents.unlock();
	}

	return result;
}

bool Agent::connectionIsReady() {
	return this->_sc->isready();
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
	Chatroom * chatroom = Chatroom::getChatroom(pkt->payload.message.chatuuid);
	if (!chatroom) {
		LOG_DEBUG("chatroom not available");
		return;
	}

	int err = chatroom->receiveMessagePacket(pkt);
	if (err) {
		LOG_DEBUG("error adding message to chatroom: %d", err);
		return;
	}
}

void Agent::receivedPayloadTypeRequestInfo(const Packet * pkt) {
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeUserInfo);

	// Ask current user to give their information
	User * curruser = Interface::current()->getuser();
	BFRetain(curruser);
	curruser->getuserinfo(&p.payload.userinfo);
	BFRelease(curruser);

	// send the information back
	this->sendPacket(&p);
}

// incoming user info to save
//
// this can be used if users wants to update their info for
// some reason
void Agent::receivedPayloadTypeUserInfo(const Packet * pkt) {
	if (!pkt)
		return;

	if (this->representsUserWithUUID(pkt->payload.userinfo.useruuid)) {
		this->updateremoteuser(&pkt->payload.userinfo);
	} else {
		// save user and save a record of the user
		// 
		// user is returned from create with retain count 2 
		// because the user class and this object owns it. We
		// will own this object and release it in our destructor.
		User * user = User::create(&pkt->payload.userinfo);
		this->setremoteuser(user);
	}
}

void Agent::receivedPayloadTypeRequestAvailableChatrooms(const Packet * pkt) {
	if (!pkt)
		return;
	
	// maybe we can user the user info to see user permissions
	//
	// nothing is being done with it right now but just passing
	// it for now

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
	
	ChatroomClient::recordChatroom(&pkt->payload.chatinfo, this);
}

void Agent::receivedPayloadTypeChatroomEnrollment(const Packet * pkt) {
	if (!pkt)
		return;

	if (!this->representsUserWithUUID(pkt->payload.enrollment.useruuid)) {
		LOG_DEBUG("%s", __func__);
		LOG_DEBUG("couldn't find user: %s",
			pkt->payload.enrollment.useruuid);
		return;
	}

	// get user with the uuid I got from the packet
	User * user = User::getuser(pkt->payload.enrollment.useruuid);

	// get chatroom
	Chatroom * chatroom = Chatroom::getChatroom(
		pkt->payload.enrollment.chatroomuuid
	);

	if (!chatroom) {
		LOG_DEBUG("couldn't find chatroom: %s",
			pkt->payload.enrollment.chatroomuuid);
		return;
	}

	BFRetain(chatroom);
	chatroom->addAgent(this);
	chatroom->addUser(user);
	BFRelease(chatroom);
}

void Agent::receivedPayloadTypeChatroomResignation(const Packet * pkt) {
	if (!pkt)
		return;

	if (!this->representsUserWithUUID(pkt->payload.enrollment.useruuid)) {
		LOG_DEBUG("%s", __func__);
		LOG_DEBUG("couldn't find user: %s",
			pkt->payload.enrollment.useruuid);
		return;
	}

	// get user with the uuid I got from the packet
	User * user = User::getuser(pkt->payload.enrollment.useruuid);

	// get chatroom
	Chatroom * chatroom = Chatroom::getChatroom(
		pkt->payload.enrollment.chatroomuuid
	);

	if (!chatroom) {
		LOG_DEBUG("couldn't find chatroom: %s",
			pkt->payload.enrollment.chatroomuuid);
		return;
	}

	BFRetain(chatroom);
	chatroom->removeAgent(this);
	chatroom->removeUser(user);
	BFRelease(chatroom);
}

int Agent::sendPacket(const Packet * pkt) {
	return this->_sc->queueData(pkt, sizeof(Packet));
}

void Agent::packetReceive(SocketEnvelope * envelope) {
	if (!envelope)
		return;

	BFRetain(envelope);

	SocketConnection * sc = envelope->connection();
	const Packet * p = (const Packet *) envelope->buf()->data();
	size_t size = envelope->buf()->size();

	if (!sc || !p) 
		return;
	else if (size != sizeof(Packet)) {
		LOG_WRITE("size of incoming data does not meet expectations: %ld != %ld", size, sizeof(Packet));
		return;
	}

	Agent * agent = Agent::getAgentForConnection(sc);
	if (!agent)
		return;

	BFRetain(agent);

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
	case kPayloadTypeNotifyChatroomListChanged:
		agent->receivedPayloadTypeNotifyChatroomListChanged(p);
		break;
	case kPayloadTypeChatroomResignation:
		agent->receivedPayloadTypeChatroomResignation(p);
		break;
	case kPayloadTypeNotifyQuitApp:
		agent->receivedPayloadTypeNotifyQuitApp(p);
		break;
	default:
		break;
	}

	BFRelease(agent);
	BFRelease(envelope);
}

int Agent::broadcast(const Packet * pkt) {
	agents.lock();
	List<Agent *>::Node * n = agents.unsafeget().first();
	for (; n; n = n->next()) {
		Agent * a = n->object();
		if (a)
			a->sendPacket(pkt);
	}
	agents.unlock();
	return 0;
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

