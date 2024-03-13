/**
 * author: brando
 * date: 3/13/24
 */

#include "envelope.hpp"
#include "connection.hpp"
#include <bflibcpp/bflibcpp.hpp>

SocketEnvelope::SocketEnvelope(SocketConnection * sc, size_t bufsize) {
	this->_sc = sc;
	BFRetain(this->_sc);

	this->_bufsize = bufsize;
	this->_buf = malloc(bufsize);
}

SocketEnvelope::~SocketEnvelope() {
	BFRelease(this->_sc);
	BFFree(this->_buf);
}

void SocketEnvelope::setbuf(const void * buf) {
	memcpy(this->_buf, buf, this->_bufsize);
}

const void * SocketEnvelope::buf() {
	return this->_buf;
}

size_t SocketEnvelope::bufsize() {
	return this->_bufsize;
}

SocketConnection * SocketEnvelope::connection() {
	return this->_sc;
}

