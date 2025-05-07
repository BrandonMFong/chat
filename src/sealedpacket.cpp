/**
 * author: brando
 * date: 5/24/24
 */

#include <bflibcpp/bflibcpp.hpp>
#include "sealedpacket.hpp"
#include <string.h>
#include <stdlib.h>

extern "C" {
#include <bflibc/bflibc.h>
}

using namespace BF;

SealedPacket::SealedPacket(const void * data, size_t size) {
	/*
	this->_dataPlainSize = size;
	
	this->_dataPlain = malloc(sizeof(char) * size);
	if (this->_dataPlain == NULL)
		return;

	memcpy(this->_dataPlain, data, size);
	*/
	this->_data = new Data(size, (const unsigned char *) data);
}

SealedPacket::~SealedPacket() { 
	//BFFree(this->_dataPlain);
	BFRelease(this->_data);
}

bool SealedPacket::isEncrypted() {
	return false;
}

const Data * SealedPacket::data() const {
	return this->_data;
}

/*
const void * SealedPacket::data() {
	return this->_dataPlain;
}

size_t SealedPacket::size() {
	return this->_dataPlainSize;
}
*/
