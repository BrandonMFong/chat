/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "socket.hpp"
#include <string.h>

int Office::PacketReceive(const Packet * pkt) {
	return 0;
}

int Office::MessageSend(const Message * msg) {
	Packet pkt;
	memset(&pkt, 0, sizeof(pkt));
	memcpy(&pkt.payload.message, msg, sizeof(Message));

	Socket::shared()->sendPacket(&pkt);
	return 0;
}

