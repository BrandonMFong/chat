/**
 * author: brando
 * date: 2/1/24
 */

#include "socket.hpp"
#include "server.hpp"
#include "client.hpp"
#include "chat.h"
#include "log.h"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

Socket::Socket() {

}

Socket::~Socket() {

}

Socket * Socket::create(const char mode, int * err) {
	Socket * result = NULL;
	int error = 0;

	switch (mode) {
		case SOCKET_MODE_SERVER:
			result = new Server;
			break;
		case SOCKET_MODE_CLIENT:
			result = new Client;
			break;
		default:
			result = NULL;
			break;
	}

	if (result == NULL) {
		error = 1;
	}

	if (err)
		*err = error;

	return result;
}

void Socket::inStream(void * in) {
	Socket * skt = (Socket *) in;
	
	while (1) {
		char buf[MESSAGE_BUFFER_SIZE];
        if (recv(skt->sockd, buf, sizeof(buf), 0) == -1) {
			ELog("%d\n", errno);
			break;
		}

		Packet * p = PACKET_ALLOC;
		memcpy(p->payload.message.buf, buf, MESSAGE_BUFFER_SIZE);

		skt->in.lock();
		skt->in.get().push(p);
		skt->in.unlock();
	}
}

void Socket::outStream(void * in) {
	Socket * skt = (Socket *) in;

	while (1) {
		skt->out.lock();
		// if queue is not empty, send the next message
		if (!skt->out.get().empty()) {
			// get first message
			Packet * p = skt->out.get().front();

			// pop queue
			skt->out.get().pop();

			// send buf from message
			send(skt->sockd, p->payload.message.buf, sizeof(p->payload.message.buf), 0);

			PACKET_FREE(p);
		}
		skt->out.unlock();
	}
}

