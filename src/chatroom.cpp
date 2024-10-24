/**
 * author: brando
 * date: 2/20/24
 */

#include "chatroom.hpp"
#include "office.hpp"
#include "user.hpp"
#include "log.hpp"
#include "message.hpp"
#include "interface.hpp"
#include "agentclient.hpp"
#include "packet.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>
#include "ciphersymmetric.hpp"
#include "cipherasymmetric.hpp"
#include "chat.hpp"
#include "exception.hpp"

using namespace BF;

Atomic<List<Chatroom *>> chatrooms;

void _ChatroomMessageFree(Message * m) { BFRelease(m); }
void _ChatroomRelease(Chatroom * cr) { BFRelease(cr); }
void _ChatroomReleaseUser(User * u) { BFRelease(u); }
void _ChatroomReleaseAgent(Agent * a) { BFRelease(a); }

Chatroom::Chatroom() : Object() {
#ifndef TESTING
	if (Chat::SocketGetMode() != SOCKET_MODE_SERVER) {
		String msg("Can only create a raw chatroom from server mode (current mode '%c')", Chat::SocketGetMode());
		throw Exception(msg);
	}
#endif

	uuid_generate_random(this->_uuid);
	memset(this->_name, 0, sizeof(this->_name));

	this->conversation.get().setReleaseCallback(_ChatroomMessageFree);
	this->_users.get().setReleaseCallback(_ChatroomReleaseUser);
	this->_agents.get().setReleaseCallback(_ChatroomReleaseAgent);

	// create our private key for encrypting
	// messages
	this->_cipher = (CipherSymmetric *) Cipher::create(kCipherTypeSymmetric);
	this->_cipher->genkey();
}

Chatroom::Chatroom(const uuid_t uuid) : Object() {
	uuid_copy(this->_uuid, uuid);
	memset(this->_name, 0, sizeof(this->_name));

	// setup conversation thread
	this->conversation.get().setReleaseCallback(_ChatroomMessageFree);
	
	this->_cipher = (CipherSymmetric *) Cipher::create(kCipherTypeSymmetric);
}

Chatroom::~Chatroom() { 
	BFRelease(this->_cipher);
}

int Chatroom::getChatroomsCount() {
	return chatrooms.get().count();
}

void Chatroom::clearChatroomList() {
	chatrooms.get().deleteAll();
}

const char * Chatroom::name() {
	return this->_name;
}

PayloadChatInfo ** Chatroom::getChatroomList(int * size, int * err) {
	if (!err || !size)
		return NULL;

	chatrooms.lock();
	int error = 0;
	int count = *size = chatrooms.unsafeget().count();
	PayloadChatInfo ** result = (PayloadChatInfo **) malloc(
		sizeof(PayloadChatInfo *) * count
	);

	List<Chatroom *>::Node * n = chatrooms.unsafeget().first();
	int i = 0;
	for (; n; n = n->next()) {
		Chatroom * cr = n->object();
		PayloadChatInfo * info = (PayloadChatInfo *) malloc(
			sizeof(PayloadChatInfo)
		);

		// get info for the chat room
		cr->getinfo(info);
		result[i] = info;

		// note down the sequence of this chatroom
		//
		// the receiver would want to know how many to expect
		info->seqcount = i;
		info->totalcount = count;
		i++;
	}

	*err = error;
	chatrooms.unlock();

	return result;
}

int Chatroom::getinfo(PayloadChatInfo * info) {
	if (!info)
		return 1;

	uuid_copy(info->chatroomuuid, this->_uuid);
	strncpy(info->chatroomname, this->_name, sizeof(info->chatroomname));
	return 0;
}

void Chatroom::addRoomToChatrooms(Chatroom * cr) {
	chatrooms.lock();

	if (chatrooms.unsafeget().count() == 0) {
		chatrooms.unsafeget().setReleaseCallback(_ChatroomRelease);
	}

	BFRetain(cr);
	chatrooms.unsafeget().add(cr);
	chatrooms.unlock();
}

int Chatroom::addMessage(Message * msg) {
	if (!msg) return 2;

	// msg count should have a retain count of 1 
	// so we don't need to retain	
	this->conversation.get().add(msg);
	Interface::current()->converstaionHasChanged();

	return 0;
}

void Chatroom::getuuid(uuid_t uuid) {
	uuid_copy(uuid, this->_uuid);
}

#ifdef DEBUG
#define __CHATROOM_MESSAGE_DEBUG(m) \
	LOG_DEBUG("%s: {type=%d, data='%s'}", __FUNCTION__, m.type(), m.data())
#else
#define __CHATROOM_MESSAGE_DEBUG(m)
#endif

int Chatroom::receiveMessagePacket(const Packet * pkt) {
	// chatroom will own this memory
	Message * m = new Message(pkt);
	if (!m) {
		LOG_DEBUG("couldn't create message object");
		return 1;
	}

	int err = 0;
	if (!this->_cipher->isReady()) {
		LOG_DEBUG("the chatroom cipher is not ready");
		err = 1;
	}

	if (!err) {
		if ((m->type() == kPayloadMessageTypeData) && m->decryptData(this->_cipher)) {
			LOG_DEBUG("couldn't decrypt message");
			err = 1;
		}
		__CHATROOM_MESSAGE_DEBUG((*m));
	}

	if (err) {
		BFRelease(m);
	} else {
		err = this->addMessage(m);
		if (err) {
			LOG_DEBUG("error adding message to chatroom: %d", err);
			return 1;
		}
	}

	return 0;
}

int _LoadPayloadTypeMessage(
	Packet * p, PayloadMessageType type, uuid_t chatuuid, 
	User * user, const InputBuffer & buf
) {
	if (!p || !user) {
		return 1;
	}

	memset(p, 0, sizeof(Packet));
	if (PacketSetHeader(p, kPayloadTypeMessage)) {
		return 1;
	}

	uuid_t useruuid;
	user->getuuid(useruuid);	
	if (PacketPayloadSetPayloadMessage(
			&p->payload.message,
			type, chatuuid, 
			user->username(), useruuid, buf)) {
		return 1;
	}

	return 0;
}

// i believe the messages aren't being encrypted correctly because I am not accounting for
// the size of the encrypted data.
int Chatroom::sendBuffer(PayloadMessageType type, User * user, const InputBuffer & buf) {
	if (!user) {
		return 1;
	} else if (!this->_cipher->isReady()) {
		LOG_DEBUG("the chatroom cipher is not ready");
		return 1;
	}

	// Load packet
	Packet p;
	if (_LoadPayloadTypeMessage(&p, type, this->_uuid, user, buf)) {
		LOG_DEBUG("could not load the message payload");
		return 1;
	}

	// encrypt data IIF it is a packet with some message data
	Message outbound(&p);
	__CHATROOM_MESSAGE_DEBUG(outbound);
	if ((type == kPayloadMessageTypeData) && outbound.encryptData(this->_cipher)) {
		LOG_DEBUG("could not encrypt data");
		return 1;
	}

	// send out packet
	if (this->sendPacket(outbound.packet())) {
		LOG_DEBUG("couldn't send packet");
		return 3;
	}
	
	// give chatroom this message to add to 
	// its list
	if (this->addMessage(new Message(&p))) {
		return 4;
	}

	return 0;
}

int Chatroom::sendBuffer(const InputBuffer & buf) {
	return this->sendBuffer(
		kPayloadMessageTypeData,
		Interface::current()->getuser(),
		buf
	);
}

int Chatroom::notifyAllChatroomUsersOfEnrollment(User * user) {
	return this->sendBuffer(kPayloadMessageTypeUserJoined, user, "");
}

int Chatroom::notifyAllServerUsersOfEnrollment(User * user) {
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeChatroomEnrollment);

	PayloadChatEnrollment enrollment;
	this->getuuid(enrollment.chatroomuuid);
	user->getuuid(enrollment.useruuid);
	PacketSetPayload(&p, &enrollment);

	// send to all users on server that a user joined a chatroom
	return Agent::broadcast(&p);
}

int Chatroom::notifyAllChatroomUsersOfResignation(User * user) {
	return this->sendBuffer(kPayloadMessageTypeUserLeft, user, "");
}

int Chatroom::notifyAllServerUsersOfResignation(User * user) {
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeChatroomResignation);

	PayloadChatEnrollment enrollment;
	this->getuuid(enrollment.chatroomuuid);
	user->getuuid(enrollment.useruuid);
	PacketSetPayload(&p, &enrollment);

	// send to all users on server that a user joined a chatroom
	return Agent::broadcast(&p);
}

int Chatroom::enroll(User * user) {
	return this->requestEnrollment(user);
}

int Chatroom::finalizeEnrollment(const PayloadChatroomEnrollmentForm * form) {
	if (!form) {
		return 1;
	}

	// get user with the uuid I got from the packet
	//
	// we need this user to decrypt the encrypted 
	// chatroom key
	User * user = User::getuser(form->useruuid);
	if (!user) {
		LOG_DEBUG("no user with %s", form->useruuid);
		LOG_FLUSH;
		return 2;
	}

	// capture private key for chatroom
	Data enckey(form->datasize, form->data);
	Data deckey;
	if (user->cipher()->decrypt(enckey, deckey)) {
		LOG_DEBUG("couldn't decrypt the encrypted data");
		return 3;
	}

	// save the chatroom key
	if (this->_cipher->setkey(deckey)) {
		LOG_DEBUG("couldn't save key");
		return 4;
	}

	int error = this->notifyAllServerUsersOfEnrollment(user);

	if (!error) {
		error = this->notifyAllChatroomUsersOfEnrollment(user);
	}

	if (!error) {
		// add user to list
		this->_users.lock();
		if (!this->_users.unsafeget().contains(user)) {
			BFRetain(user);
			this->_users.unsafeget().add(user);
		}
		this->_users.unlock();
	}
	
	LOG_FLUSH;

	return error;
}

int Chatroom::fillOutEnrollmentFormRequest(User * user, Packet * p) {
	if (!user || !p) return 2;

	memset(p, 0, sizeof(Packet));
	PacketSetHeader(p, kPayloadTypeChatroomEnrollmentForm);

	PayloadChatroomEnrollmentForm form;
	form.type = 0; // request
	user->getuuid(form.useruuid);
	this->getuuid(form.chatroomuuid);

	Data pub;
	const CipherAsymmetric * c = (const CipherAsymmetric *) user->cipher();
	c->getPublicKey(pub);

	// i am afraid of this
	if (pub.size() > sizeof(form.data)) {
		LOG_WRITE("pub key size vs available buffer size in packet, %ld != %ld", pub.size(), sizeof(form.data));
		return 1;
	}

	// transfer the public key
	memcpy(form.data, pub.buffer(), pub.size());
	form.datasize = pub.size();
	
	PacketSetPayload(p, &form);

	return 0;
}

/**
 * this will use the public key in the form to encrypt the symmetric key
 */
int _encryptPrivateKey(PayloadChatroomEnrollmentForm * form, CipherSymmetric * c, Data & res) {
	if (!form || !c)
		return 1;

	// get private key	
	Data key;
	if (c->getkey(key)) {
		LOG_DEBUG("could not get private key");
		return 2;
	}

	// create asym cipher
	CipherAsymmetric * ac = (CipherAsymmetric *) Cipher::create(kCipherTypeAsymmetric);
	if (!ac) {
		return 3;
	}

	Data pubkey(form->datasize, form->data);
	if (ac->setPublicKey(pubkey)) {
		return 4;
	}

	// encrypt our private key
	if (ac->encrypt(key, res)) {
		return 5;
	}

	BFDelete(ac);
	
	return 0;
}

int Chatroom::fillOutEnrollmentFormResponse(PayloadChatroomEnrollmentForm * form) {
	if (!form) {
		return 1;
	} else if (form->type != 0) {
		// we should only receive a request type form
		return 2;
	}

	// encrypt private key
	Data enckey;
	int err = _encryptPrivateKey(form, this->_cipher, enckey);
	if (err) {
		return err;
	}

	// transfer the encrypted private key
	memcpy(form->data, enckey.buffer(), enckey.size());
	form->datasize = enckey.size();
	
	// modify form to reflect response type
	form->type = 1; // response
	form->approved = true;

	return err;
}

int Chatroom::resign(User * user) {
	int error = this->notifyAllServerUsersOfResignation(user);

	if (!error) {
		error = this->notifyAllChatroomUsersOfResignation(user);
	}

	if (!error) {
		// add user to list
		this->_users.lock();
		if (this->_users.unsafeget().contains(user)) {
			this->_users.unsafeget().pluckObject(user);
			BFRelease(user);
		}
		this->_users.unlock();
	}

	return error;
}

int Chatroom::agentAddRemove(const char action, Agent * a) {
	int error = 0;
	// finally add agent to list
	this->_agents.lock();
	if (action == 'a') {
		if (!this->_agents.unsafeget().contains(a)) {
			error = this->_agents.unsafeget().add(a);
			BFRetain(a);
		}
	} else if (action == 'r') {
		if (this->_agents.unsafeget().contains(a)) {
			error = this->_agents.unsafeget().pluckObject(a);
			BFRelease(a);
		}
	}
	this->_agents.unlock();

	return error;
}

int Chatroom::userAddRemove(const char action, User * user) {
	int error = 0;
	// add user from agent to list
	this->_users.lock();
	if (action == 'a') {
		if (!this->_users.unsafeget().contains(user)) {
			error = this->_users.unsafeget().add(user);
			BFRetain(user);
		}
	} else if (action == 'r') {
		if (this->_users.unsafeget().contains(user)) {
			error = this->_users.unsafeget().pluckObject(user);
			BFRelease(user);
		}
	}
	this->_users.unlock();
	return error;
}


int Chatroom::addAgent(Agent * a) {
	return this->agentAddRemove('a', a);
}

int Chatroom::addUser(User * u) {
	return this->userAddRemove('a', u);
}

int Chatroom::removeAgent(Agent * a) {
	return this->agentAddRemove('r', a);
}

int Chatroom::removeUser(User * u) {
	return this->userAddRemove('r', u);
}

Chatroom * Chatroom::getChatroom(const uuid_t chatroomuuid) {
	Chatroom * room = NULL;
	chatrooms.lock();
	List<Chatroom *>::Node * n = chatrooms.unsafeget().first();
	for (; n != NULL; n = n->next()) {
		Chatroom * troom = n->object();
		uuid_t uuid;
		troom->getuuid(uuid);
		if (!uuid_compare(uuid, chatroomuuid)) {
			room = troom;
			break;
		}
	}
	chatrooms.unlock();

	return room;
}

