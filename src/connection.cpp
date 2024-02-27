/**
 * author: brando
 * date: 2/26/24
 */

#include "connection.hpp"
#include "socket.hpp"
#include "log.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>

using namespace BF;

void SocketConnection::ReleaseConnection(SocketConnection * sc) {
	Delete(sc);
}

SocketConnection::SocketConnection(int sd, Socket * sktref) : Object() {
	this->_sd = sd;
	this->_sktref = sktref;
	BFRetain(this->_sktref);
	uuid_generate_random(this->_uuid);
}

SocketConnection::~SocketConnection() {
	BFRelease(this->_sktref);
}

void SocketConnection::closeConnection() {
	shutdown(this->_sd, SHUT_RDWR);
	close(this->_sd);
}

bool SocketConnection::isready() {
	return this->_isready.get();
}

const char SocketConnection::mode() {
	return this->_sktref->mode();
}

int SocketConnection::queueData(const void * data, size_t size) {
	if (!data) return -2;

	// make envelope
	struct Socket::Envelope * envelope = (struct Socket::Envelope *) malloc(sizeof(struct Socket::Envelope *));
	if (!envelope) return -2;

	envelope->sc = this;
	envelope->buf.data = malloc(size);
	envelope->buf.size = size;
	memcpy(envelope->buf.data, data, size);

	// queue up envelope
	int error = this->_sktref->_outq.get().push(envelope);
	return error;
}

int SocketConnection::sendData(const void * b) {
	const Socket::Buffer * buf = (const Socket::Buffer *) b;
	send(this->_sd, buf->data, buf->size, 0);

	return 0;
}

int SocketConnection::recvData(void * b) {
	Socket::Buffer * buf = (Socket::Buffer *) b;
	buf->size = recv(this->_sd, buf->data, this->_sktref->_bufferSize, 0);
	if (buf->size == -1) {
		LOG_DEBUG("recv error %d", errno);
		return errno;
	} else if (buf->size == 0) {
		LOG_DEBUG("recv received 0");
		return -1;
	}

	return 0;
}

