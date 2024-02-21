/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"
#include <string.h>

using namespace BF;

int Office::PacketReceive(const Packet * p) {
	if (!p) return 1;
	const Message * m = &p->payload.message;

	Chatroom * chatroom = ChatDirectory::shared()->getChatroom(m->chatuuid);
	if (!chatroom) return 2;

	return chatroom->addMessage(m);
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

