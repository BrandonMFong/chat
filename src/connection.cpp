/**
 * author: brando
 * date: 2/26/24
 */

#include "connection.hpp"
#include <bflibcpp/bflibcpp.hpp>

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

