/**
 * author: brando
 * date: 2/26/24
 */

#include "connection.hpp"

SocketConnection::SocketConnection(int sd) : Object() {
	this->_sd = sd;
}

SocketConnection::~SocketConnection() {

}

