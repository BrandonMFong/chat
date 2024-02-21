/**
 * author: brando
 * date: 2/1/24
 */

#include "socket.hpp"
#include "server.hpp"
#include "client.hpp"
#include "chat.h"
#include "log.hpp"
#include "office.hpp"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include <bflibc/bflibc.h>

Socket * _sharedSocket = NULL;

Socket * Socket::shared() {
	return _sharedSocket;
}

Socket::Socket() { 
	this->_tidin = NULL;
	this->_tidinpop = NULL;
	this->_tidout = NULL;

	this->_callback = NULL;
	this->_stopStreams = false;

	_sharedSocket = this;
}

Socket::~Socket() {
	LOG_DEBUG("socket destroyed");
	BFThreadAsyncDestroy(this->_tidin);
	BFThreadAsyncDestroy(this->_tidinpop);
	BFThreadAsyncDestroy(this->_tidout);
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

void Socket::queueCallback(void * in) {
	LOG_DEBUG("> %s", __func__);
	Socket * skt = (Socket *) in;

	BFRetain(skt);

	while (!BFThreadAsyncIsCanceled(skt->_tidinpop)) {
		if (skt->_stopStreams.get())
			break;

		skt->_inq.lock();
		// if queue is not empty, send the next message
		if (!skt->_inq.get().empty()) {
			// get first message
			Packet * p = skt->_inq.get().front();

			if (p) {
				//skt->_callback(*p);
				Office::PacketReceive(p);
			}

			// pop queue
			skt->_inq.get().pop();

			PACKET_FREE(p);
		}
		skt->_inq.unlock();
	}

	BFRelease(skt);

	LOG_DEBUG("< %s", __func__);
}

void Socket::inStream(void * in) {
	LOG_DEBUG("> %s", __func__);
	Socket * skt = (Socket *) in;

	BFRetain(skt);
	
	while (!BFThreadAsyncIsCanceled(skt->_tidin)) {
		if (skt->_stopStreams.get())
			break;

		Packet buf;
		size_t bufsize = recv(skt->descriptor(), &buf, sizeof(Packet), 0);
        if (bufsize == -1) {
			LOG_ERROR("%d", errno);
			break;
		} else if (bufsize == 0) {
			LOG_DEBUG("recv received 0");
			break;
		}

		Packet * p = PACKET_ALLOC;
		memcpy(p, &buf, sizeof(Packet));
		
		LOG_DEBUG("incoming {packet = {message = {%f, \"%s\", \"%s\"}}}",
			p->payload.message.time,
			p->payload.message.username,
			p->payload.message.buf
		);

		skt->_inq.get().push(p);
	}

	BFRelease(skt);
	LOG_DEBUG("< %s", __func__);
}

void Socket::outStream(void * in) {
	LOG_DEBUG("> %s", __func__);
	Socket * skt = (Socket *) in;

	BFRetain(skt);

	while (!BFThreadAsyncIsCanceled(skt->_tidinpop)) {
		if (skt->_stopStreams.get())
			break;

		skt->_outq.lock();
		// if queue is not empty, send the next message
		if (!skt->_outq.get().empty()) {
			// get first message
			Packet * p = skt->_outq.get().front();

			LOG_DEBUG("outgoing {packet = {message = {%f, \"%s\", \"%s\"}}}",
				p->payload.message.time,
				p->payload.message.username,
				p->payload.message.buf
			);

			// pop queue
			skt->_outq.get().pop();

			// send buf from message
			send(skt->descriptor(), p, sizeof(Packet), 0);

			PACKET_FREE(p);
		}
		skt->_outq.unlock();
	}

	BFRelease(skt);
	LOG_DEBUG("< %s", __func__);
}

int Socket::startIOStreams() {
	this->_tidinpop = BFThreadAsync(Socket::queueCallback, (void *) this);
	this->_tidin = BFThreadAsync(Socket::inStream, (void *) this);
	this->_tidout = BFThreadAsync(Socket::outStream, (void *) this);

	return 0;
}

int Socket::start() {
	if (this->_callback == NULL) {
		LOG_DEBUG("please set callback for instream before starting socket\n");
		return -1;
	}
	this->_start();
	return 0;
}

int Socket::stop() {
	int error = this->_stop();

	if (!error && this->_tidin) {
		error = BFThreadAsyncCancel(this->_tidin);
		BFThreadAsyncWait(this->_tidin);
	}

	if (!error && this->_tidinpop) {
		error = BFThreadAsyncCancel(this->_tidinpop);
		BFThreadAsyncWait(this->_tidinpop);
	}

	if (!error && this->_tidout) {
		error = BFThreadAsyncCancel(this->_tidout);
		BFThreadAsyncWait(this->_tidout);
	}

	return error;
}

int Socket::sendPacket(const Packet * pkt) {
	if (!pkt) return -2;

	Packet * p = PACKET_ALLOC;
	if (!p) return -2;

	memcpy(p, pkt, sizeof(Packet));

	int error = this->_outq.get().push(p);
	return error;
}

void Socket::setQueueCallback(void (* callback)(const Packet &)) {
	this->_callback = callback;
}

