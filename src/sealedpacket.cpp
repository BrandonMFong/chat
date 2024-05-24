/**
 * author: brando
 * date: 5/24/24
 */

#include <bflibcpp/bflibcpp.hpp>
#include "sealedpacket.hpp"
#include <string.h>
#include <stdlib.h>

SealedPacket::SealedPacket(const void * data, size_t size) {
	this->_dataPlainSize = size;
	
	this->_dataPlain = malloc(sizeof(char) * size);
	if (this->_dataPlain == NULL)
		return;

	memcpy(this->_dataPlain, data, size);
}

SealedPacket::~SealedPacket() { 
	BFFree(this->_dataPlain);
}

bool SealedPacket::isEncrypted() {
	return false;
}

const void * SealedPacket::data() {
	return this->_dataPlain;
}

size_t SealedPacket::size() {
	return this->_dataPlainSize;
}

