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
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<List<Chatroom *>> chatrooms;

void _ChatroomMessageFree(Message * m) {
	Delete(m);
}

void _ChatroomRelease(Chatroom * cr) {
	BFRelease(cr);
}

Chatroom::Chatroom() : Object() {
	this->updateConversation = false;
	uuid_generate_random(this->_uuid);
	memset(this->_name, 0, sizeof(this->_name));

	// setup conversation thread
	this->conversation.get().setDeallocateCallback(_ChatroomMessageFree);
}

Chatroom::~Chatroom() {

}

int Chatroom::getChatroomsCount() {
	return chatrooms.get().count();
}

PayloadChatroomInfoBrief ** Chatroom::getChatroomList(int * size, int * err) {
	if (!err || !size)
		return NULL;

	chatrooms.lock();
	int error = 0;
	int count = *size = chatrooms.unsafeget().count();
	PayloadChatroomInfoBrief ** result = (PayloadChatroomInfoBrief **) malloc(
		sizeof(PayloadChatroomInfoBrief *) * count
	);

	List<Chatroom *>::Node * n = chatrooms.unsafeget().first();
	int i = 0;
	for (; n; n = n->next()) {
		Chatroom * cr = n->object();
		PayloadChatroomInfoBrief * info = (PayloadChatroomInfoBrief *) malloc(
			sizeof(PayloadChatroomInfoBrief)
		);

		// get info for the chat room
		cr->getinfobrief(info);
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

int Chatroom::getinfobrief(PayloadChatroomInfoBrief * info) {
	if (!info)
		return 1;

	uuid_copy(info->chatroomuuid, this->_uuid);
	strncpy(info->chatroomname, this->_name, sizeof(info->chatroomname));
	return 0;
}

int Chatroom::recordChatroom(const PayloadChatroomInfoBrief * info) {
	if (!info)
		return 1;

	Chatroom * chatroom = new Chatroom;
	if (!chatroom)
		return 2;

	uuid_copy(chatroom->_uuid, info->chatroomuuid);
	memcpy(chatroom->_name, info->chatroomname, sizeof(chatroom->_name));

#ifdef DEBUG
	char uuidstr[UUID_STR_LEN];
	uuid_unparse(chatroom->_uuid, uuidstr);
	LOG_DEBUG("new chat room: %s - %s", chatroom->_name, uuidstr);
#endif

	Chatroom::addRoomToChatrooms(chatroom);

	BFRelease(chatroom);

	return 0;
}

void Chatroom::addRoomToChatrooms(Chatroom * cr) {
	chatrooms.lock();

	if (chatrooms.unsafeget().count() == 0) {
		chatrooms.unsafeget().setDeallocateCallback(_ChatroomRelease);
	}

	BFRetain(cr);
	chatrooms.unsafeget().add(cr);
	chatrooms.unlock();
}

int Chatroom::addMessage(Message * msg) {
	if (!msg) return 2;
	
	this->conversation.get().add(msg);
	this->updateConversation = true;

	return 0;
}

void Chatroom::getuuid(uuid_t uuid) {
	uuid_copy(uuid, this->_uuid);
}

int Chatroom::sendBuffer(const InputBuffer * buf) {
	Packet p;

	memset(&p, 0, sizeof(p));

	// load buffer 
	strncpy(
		p.payload.message.data,
		buf->cString(),
		sizeof(p.payload.message.data)
	);

	// username
	strncpy(
		p.payload.message.username,
		Interface::GetCurrentUser()->username(),
		sizeof(p.payload.message.username)
	);

	uuid_t uuid;
	Interface::GetCurrentUser()->getuuid(uuid);	

	// user uuid
	uuid_copy(p.payload.message.useruuid, uuid);

	// chatroom uuid
	uuid_copy(p.payload.message.chatuuid, this->_uuid);

	// time
	p.header.time = BFTimeGetCurrentTime();

	// set payload type
	p.header.type = kPayloadTypeMessage;

	// give chatroom this message to add to 
	// its list
	this->addMessage(new Message(&p));

	// send out message to the rest of the chatrooms.in the 
	// chatroom
	return Office::PacketSend(&p);
}

Chatroom * Chatroom::getChatroom(uuid_t chatroomuuid) {
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

