/**
 * author: brando
 * date: 2/22/24
 */

#include "investigate.hpp"
#include "socket.hpp"

int _InvestigateNewClient(int c) {
	return 0;
}

int _InvestigateNewServer(int s) {
	return 0;
}

int Investigate::NewConnection(int d) {
	Socket * skt = Socket::shared();
	if (!skt)
		return 1;

	switch (skt->mode()) {
	case SOCKET_MODE_SERVER:
		return _InvestigateNewServer(d);
	case SOCKET_MODE_CLIENT:
		return _InvestigateNewClient(d);
	default:
		return 2;
	}
}

