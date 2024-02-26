/**
 * author: brando
 * date: 2/23/24
 */

#include "message.hpp"
#include <string.h>

using namespace BF;

Message::Message(const Packet * pkt) : Object() {
	if (pkt) {
		memcpy(&this->_packet, pkt, sizeof(this->_packet));
	}
}

Message::~Message() {

}

const char * Message::username() const {
	return this->_packet.payload.message.username;
}

const char * Message::data() const {
	return this->_packet.payload.message.data;
}

const char * Message::chatuuid() const {
	return this->_packet.payload.message.chatuuid;
}

