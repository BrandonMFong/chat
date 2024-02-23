/**
 * author: brando
 * date: 2/23/24
 */

#include "message.hpp"
#include <string.h>

using namespace BF;

Message::Message(const Packet * pkt) : Object() {
	strncpy(this->_data, pkt->data, sizeof(this->_data));
	strncpy(this->_sendername, pkt->header.username, sizeof(this->_sendername));
	strncpy(this->_chatroomuuid, pkt->header.chatuuid, sizeof(this->_chatroomuuid));

}

Message::~Message() {

}

const char * Message::username() const {
	return this->_sendername;
}

const char * Message::data() const {
	return this->_data;
}

const char * Message::chatuuid() const {
	return this->_chatroomuuid;
}

