/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"
#include "log.hpp"
#include <string.h>

using namespace BF;

void Office::PacketReceive(const void * buf, size_t size) {
	const Packet * p = (const Packet *) buf;
	if (!p) {
		LOG_DEBUG("data is null");
		return;
	}

	const Message * m = &p->payload.message;

	Chatroom * chatroom = ChatDirectory::shared()->getChatroom(m->chatuuid);
	if (!chatroom) {
		LOG_DEBUG("chatroom not available");
		return;
	}

	int err = chatroom->addMessage(m);
	if (!err) {
		LOG_DEBUG("error adding message to chatroom: %d", err);
		return;
	}
}

int Office::MessageSend(const Message * m) {
	Packet p;

	// clear packet
	memset(&p, 0, sizeof(p));
	
	// load message
	memcpy(&p.payload.message, m, sizeof(p.payload.message));

	// send to socket
	return Socket::shared()->sendPacket(&p);
}

