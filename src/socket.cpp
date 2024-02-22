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

using namespace BF;

Socket * _sharedSocket = NULL;

Socket * Socket::shared() {
	return _sharedSocket;
}

Socket::Socket() { 
	this->_tidq = NULL;
	this->_tidout = NULL;

	_sharedSocket = this;
}

Socket::~Socket() {
	LOG_DEBUG("socket destroyed");
	
	this->_tidin.lock();
	List<BFThreadAsyncID>::Node * n = this->_tidin.unsafeget().first();
	for (; n != NULL; n = n->next()) {
		BFThreadAsyncDestroy(n->object());
	}
	this->_tidin.unlock();

	BFThreadAsyncDestroy(this->_tidq);
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

	while (!BFThreadAsyncIsCanceled(skt->_tidq)) {
		skt->_inq.lock();
		// if queue is not empty, send the next message
		if (!skt->_inq.unsafeget().empty()) {
			// get first message
			Packet * p = skt->_inq.unsafeget().front();

			if (p) {
				Office::PacketReceive(p);
			}

			// pop queue
			skt->_inq.unsafeget().pop();

			PACKET_FREE(p);
		}
		skt->_inq.unlock();
	}

	BFRelease(skt);

	LOG_DEBUG("< %s", __func__);
}

/**
 * used for inStream
 *
 * allows us to run dedicated threads for each socket connection
 */
class InStreamTools : public Object {
public:
	int mainConnection;
	Socket * socket;
};

void Socket::inStream(void * in) {
	LOG_DEBUG("> %s", __func__);
	InStreamTools * tools = (InStreamTools *) in;
	const int sd = tools->mainConnection;
	Socket * skt = tools->socket;
	BFThreadAsyncID tid = BFThreadAsyncGetID();

	BFRetain(tools);
	BFRetain(skt);
	
	while (!BFThreadAsyncIsCanceled(tid)) {
		Packet buf;
		size_t bufsize = recv(sd, &buf, sizeof(Packet), 0);
        if (bufsize == -1) {
			LOG_DEBUG("recv returned %d", errno);
			break;
		} else if (bufsize == 0) {
			LOG_DEBUG("recv received 0");
			break;
		}

		Packet * p = PACKET_ALLOC;
		memcpy(p, &buf, sizeof(Packet));
		
		LOG_DEBUG("incoming {packet = {message = {%f, \"%s\", \"%s\", \"%s\"}}}",
			p->payload.message.time,
			p->payload.message.username,
			p->payload.message.chatuuid,
			p->payload.message.buf
		);

		skt->_inq.get().push(p);
	}

	BFRelease(skt);
	BFRelease(tools);

	LOG_DEBUG("< %s", __func__);
}

void Socket::outStream(void * in) {
	LOG_DEBUG("> %s", __func__);
	Socket * skt = (Socket *) in;

	BFRetain(skt);

	while (!BFThreadAsyncIsCanceled(skt->_tidq)) {
		skt->_outq.lock();
		// if queue is not empty, send the next message
		if (!skt->_outq.unsafeget().empty()) {
			// get first message
			Packet * p = skt->_outq.unsafeget().front();

			LOG_DEBUG("outgoing {packet = {message = {%f, \"%s\", \"%s\", \"%s\"}}}",
				p->payload.message.time,
				p->payload.message.username,
				p->payload.message.chatuuid,
				p->payload.message.buf
			);

			// pop queue
			skt->_outq.unsafeget().pop();

			// send buf from message to each connection
			skt->_connections.lock();
			for (int i = 0; i < skt->_connections.unsafeget().count(); i++) {
				send(skt->_connections.unsafeget()[i], p, sizeof(Packet), 0);
			}
			skt->_connections.unlock();

			PACKET_FREE(p);
		}
		skt->_outq.unlock();
	}

	BFRelease(skt);
	LOG_DEBUG("< %s", __func__);
}

// called by subclasses whenever they get a new connection
int Socket::startInStreamForConnection(int sd) {
	InStreamTools * tools = new InStreamTools;
	tools->mainConnection = sd;
	tools->socket = this;

	BFThreadAsyncID tid = BFThreadAsync(Socket::inStream, (void *) tools);
	this->_tidin.get().add(tid);

	// wait for at leat one of the threads to run
	// so they can retain the io stream tools
	//
	// we want to make sure we release tools 
	// in the right time
	while (Object::retainCount(tools) < 2) {}
	BFRelease(tools);

	return 0;
}

int Socket::start() {
	this->_start();

	this->_tidq = BFThreadAsync(Socket::queueCallback, (void *) this);

	// out stream uses `send`
	//
	// we can use this object and iterate through the connections
	// array to send the same packet at the top of the queue
	this->_tidout = BFThreadAsync(Socket::outStream, (void *) this);

	return 0;
}

int Socket::stop() {
	int error = 0;

	this->_connections.lock();
	for (int i = 0; i < this->_connections.unsafeget().count(); i++) {
		shutdown(this->_connections.unsafeget()[i], SHUT_RDWR);
		close(this->_connections.unsafeget()[i]);
	}
	this->_connections.unlock();

	error = this->_stop();

	if (!error) {
		this->_tidin.lock();
		List<BFThreadAsyncID>::Node * n = this->_tidin.unsafeget().first();
		for (; n != NULL; n = n->next()) {
			error = BFThreadAsyncCancel(n->object());
			BFThreadAsyncWait(n->object());
			if (error) {
				LOG_DEBUG("error code for cancel attempt: %d", error);
				break;
			}
		}
		this->_tidin.unlock();
	}

	if (!error && this->_tidq) {
		error = BFThreadAsyncCancel(this->_tidq);
		BFThreadAsyncWait(this->_tidq);
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

