/**
 * author: brando
 * date: 2/26/24
 */

#include "connection.hpp"
#include <bflibcpp/delete.hpp>

void SocketConnection::ReleaseConnection(SocketConnection * sc) {
	Delete(sc);
}

SocketConnection::SocketConnection(int sd) : Object() {
	this->_sd = sd;
	uuid_generate_random(this->_uuid);
}

SocketConnection::~SocketConnection() {

}

int SocketConnection::descriptor() const {
	return this->_sd;
}

