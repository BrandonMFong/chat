/**
 * author: brando
 * date: 2/23/24
 */

#include "message.hpp"
#include "cipher.hpp"
#include "log.hpp"
#include <string.h>

using namespace BF;

Message::Message(const Packet * pkt) : Object() {
	if (pkt) {
		memcpy(&this->_packet, pkt, sizeof(this->_packet));
	}
}

Message::~Message() {

}

int Message::decryptData(const Cipher * cipher) {
	if (!cipher) {
		return 1;
	}
	
	// decrypt the message
	Data enc(sizeof(this->_packet.payload.message.data),
			(unsigned char *) this->_packet.payload.message.data);
	Data dec;
	int err = cipher->decrypt(enc, dec);
	if (err) {
		LOG_DEBUG("couldn't decrypt message: %d", err);
		return err;
	}

	strncpy(this->_packet.payload.message.data,
			(char *) dec.buffer(),
			sizeof(this->_packet.payload.message.data));

	return 0;
}

int Message::encryptData(const Cipher * cipher) {
	if (!cipher) {
		return 1;
	}
	
	// encrypt the message
	Data plain(sizeof(this->_packet.payload.message.data),
			(unsigned char *) this->_packet.payload.message.data);
	Data enc;
	int err = cipher->encrypt(plain, enc);
	if (err) {
		LOG_DEBUG("couldn't encrypt message: %d", err);
		return err;
	}

	strncpy(this->_packet.payload.message.data,
			(char *) enc.buffer(),
			sizeof(this->_packet.payload.message.data));


	return 0;
}

const Packet * Message::packet() const {
	return &this->_packet;
}

const char * Message::username() const {
	return this->_packet.payload.message.username;
}

const char * Message::data() const {
	return this->_packet.payload.message.data;
}

void Message::getuuidchatroom(uuid_t uuid) const {
	uuid_copy(uuid, this->_packet.payload.message.chatuuid);
}

void Message::getuuiduser(uuid_t uuid) const {
	uuid_copy(uuid, this->_packet.payload.message.useruuid);
}

PayloadMessageType Message::type() const {
	return this->_packet.payload.message.type;
}

