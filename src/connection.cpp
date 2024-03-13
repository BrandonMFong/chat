/**
 * author: brando
 * date: 2/26/24
 */

#include "connection.hpp"
#include "socket.hpp"
#include "envelope.hpp"
#include "buffer.hpp"
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

void SocketConnection::getuuid(uuid_t uuid) {
	memcpy(uuid, this->_uuid, sizeof(uuid_t));
}

int SocketConnection::queueData(const void * data, size_t size) {
	if (!data) return -2;

	// make envelope
	SocketEnvelope * envelope = new SocketEnvelope(this, size);
	if (!envelope) return -2;

	memcpy(envelope->buf()->_data, data, envelope->buf()->size());

	// queue up envelope
	int error = this->_sktref->_outq.get().push(envelope);
	return error;
}

int SocketConnection::sendData(const SocketBuffer * buf) {
	if (!buf)
		return 1;
	
	send(this->_sd, buf->data(), buf->size(), 0);

	return 0;
}

int SocketConnection::recvData(SocketBuffer * buf) {
	if (!buf)
		return 1;

	buf->_size = recv(this->_sd, buf->_data, this->_sktref->_bufferSize, 0);
	if (buf->_size == -1) {
		LOG_DEBUG("recv error %d", errno);
		return errno;
	} else if (buf->_size == 0) {
		LOG_DEBUG("recv received 0");
		return -1;
	}

	return 0;
}

