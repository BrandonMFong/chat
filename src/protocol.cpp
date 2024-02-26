/**
 * author: brando
 * date: 2/22/24
 *
 * https://en.wikipedia.org/wiki/Session_Initiation_Protocol
 */

#include "protocol.hpp"
#include "socket.hpp"
#include "typepacket.h"

int Protocol::Handshake(const SocketConnection * d) {
	Socket * skt = Socket::shared();
	if (!skt)
		return 1;

	return 0;
}

