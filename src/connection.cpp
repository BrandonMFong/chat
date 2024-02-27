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
#include <unistd.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>

using namespace BF;

void SocketConnection::ReleaseConnection(SocketConnection * sc) {
	Delete(sc);
}

SocketConnection::SocketConnection(int sd) : Object() {
	this->_sd = sd;
	uuid_generate_random(this->_uuid);
}

SocketConnection::~SocketConnection() {

}

int SocketConnection::descriptor() {
	return this->_sd;
}

bool SocketConnection::isready() {
	return this->_isready.get();
}

int SocketConnection::queueData(const void * data, size_t size) {
	if (!data) return -2;

	// make struct
	struct Socket::Buffer * buf = (struct Socket::Buffer *) malloc(sizeof(struct Socket::Buffer));
	if (!buf) return -2;

	// make data
	buf->data = malloc(size);
	buf->size = size;
	memcpy(buf->data, data, size);

	// queue up buffer
	int error = Socket::shared()->_outq.get().push(buf);
	return error;
}

int SocketConnection::sendData(const void * b) {
	const Socket::Buffer * buf = (const Socket::Buffer *) b;
	send(this->_sd, buf->data, buf->size, 0);

	return 0;
}

int SocketConnection::recvData(void * b) {
	Socket::Buffer * buf = (Socket::Buffer *) b;
	buf->size = recv(this->_sd, buf->data, Socket::shared()->_bufferSize, 0);
	if (buf->size == -1) {
		LOG_DEBUG("recv error %d", errno);
		return errno;
	} else if (buf->size == 0) {
		LOG_DEBUG("recv received 0");
		return -1;
	}

	return 0;
}

