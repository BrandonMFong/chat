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

int Office::PacketReceive(const Packet * pkt) {
	return 0;
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

