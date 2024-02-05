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
#include <unistd.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include <bflibc/bflibc.h>

Socket::Socket() { 
	this->_tidinpush = NULL;
	this->_tidinpop = NULL;
	this->_tidout = NULL;
	this->_callback = NULL;
}

Socket::~Socket() { }

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

void Socket::inStreamQueuePush(void * in) {
	Socket * skt = (Socket *) in;

	BFRetain(skt);
	
	while (1) {
		char buf[MESSAGE_BUFFER_SIZE];
		size_t bufsize = recv(skt->descriptor(), buf, sizeof(buf), 0);
        if (bufsize == -1) {
			ELog("%d\n", errno);
			break;
		} else if (bufsize == 0) {
			DLog("recv received 0\n");
			break;
		}

		Packet * p = PACKET_ALLOC;
		memcpy(p->payload.message.buf, buf, MESSAGE_BUFFER_SIZE);

		skt->_inq.lock();
		skt->_inq.get().push(p);
		skt->_inq.unlock();
	}

	BFRelease(skt);
}

void Socket::inStreamQueuePop(void * in) {
	Socket * skt = (Socket *) in;

	while (1) {
		skt->_inq.lock();
		// if queue is not empty, send the next message
		if (!skt->_inq.get().empty()) {
			// get first message
			Packet * p = skt->_inq.get().front();

			if (p) {
				skt->_callback(*p);
			}

			// pop queue
			skt->_inq.get().pop();

			PACKET_FREE(p);
		}
		skt->_inq.unlock();
	}
}

void Socket::outStream(void * in) {
	Socket * skt = (Socket *) in;

	BFRetain(skt);

	while (1) {
		skt->_outq.lock();
		// if queue is not empty, send the next message
		if (!skt->_outq.get().empty()) {
			// get first message
			Packet * p = skt->_outq.get().front();

			// pop queue
			skt->_outq.get().pop();

			// send buf from message
			send(skt->descriptor(), p->payload.message.buf, sizeof(p->payload.message.buf), 0);

			PACKET_FREE(p);
		}
		skt->_outq.unlock();
	}

	BFRelease(skt);
}

int Socket::startIOStreams() {
	this->_tidinpush = BFThreadAsync(Socket::inStreamQueuePush, (void *) this);
	this->_tidinpop = BFThreadAsync(Socket::inStreamQueuePop, (void *) this);
	this->_tidout = BFThreadAsync(Socket::outStream, (void *) this);

	return 0;
}

int Socket::start() {
	this->_start();
	return 0;
}

int Socket::stop() {
	int error = this->_stop();

	if (!error && this->_tidinpush) {
		error = BFThreadAsyncCancel(this->_tidinpush);
		BFThreadAsyncIDDestroy(this->_tidinpush);
	}

	if (!error && this->_tidinpop) {
		error = BFThreadAsyncCancel(this->_tidinpop);
		BFThreadAsyncIDDestroy(this->_tidinpop);
	}

	if (!error && this->_tidout) {
		error = BFThreadAsyncCancel(this->_tidout);
		BFThreadAsyncIDDestroy(this->_tidout);
	}

	return error;
}

int Socket::sendPacket(const Packet * pkt) {
	if (!pkt) return -2;

	Packet * p = PACKET_ALLOC;
	if (!p) return -2;

	memcpy(p, pkt, sizeof(Packet));

	this->_outq.lock();
	int error = this->_outq.get().push(p);
	this->_outq.unlock();
	return error;
}

void Socket::setInStreamCallback(void (* callback)(const Packet &)) {
	this->_callback = callback;
}

