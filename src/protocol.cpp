/**
 * author: brando
 * date: 2/22/24
 *
 * https://en.wikipedia.org/wiki/Session_Initiation_Protocol
 */

#include "protocol.hpp"
#include "socket.hpp"
#include "typepacket.h"

int _ProtocolNewClient(int c) {
	return 0;
}

int _ProtocolNewServer(int s) {
	return 0;
}

int Protocol::Handshake(int d) {
	Socket * skt = Socket::shared();
	if (!skt)
		return 1;

	switch (skt->mode()) {
	case SOCKET_MODE_SERVER:
		return _ProtocolNewServer(d);
	case SOCKET_MODE_CLIENT:
		return _ProtocolNewClient(d);
	default:
		return 2;
	}
}

