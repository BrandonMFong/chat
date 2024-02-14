/**
 * author: brando
 * date: 2/14/24
 */

#include "inputbuffer.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "user.hpp"

using namespace BF;

int InputBuffer::loadPacket(Packet * pkt) {
	if (!pkt) return 50;

	strncpy(pkt->payload.message.buf, this->cString(), sizeof(pkt->payload.message.buf));
	strncpy(pkt->payload.message.username, User::current()->username(), sizeof(pkt->payload.message.username));
	Time * t = Time::createCurrent();
	pkt->payload.message.time = t->epoch();
	Delete(t);

	return 0;
}

