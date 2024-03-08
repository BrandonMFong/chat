/**
 * author: brando
 * date: 2/1/24
 */

#include "socket.hpp"
#include "server.hpp"
#include "connection.hpp"
#include "client.hpp"
#include "log.hpp"
#include "office.hpp"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
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

	this->_cbinstream = NULL;
	this->_cbnewconn = NULL;

	this->_bufferSize = 0;

	this->_connections.get().setReleaseCallback(SocketConnection::ReleaseConnection);

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

void Socket::setBufferSize(size_t size) {
	this->_bufferSize = size;
}

void Socket::setNewConnectionCallback(int (* cb)(SocketConnection * sc)) {
	this->_cbnewconn = cb;
}

void Socket::setInStreamCallback(void (* cb)(SocketConnection * sc, const void * buf, size_t size)) {
	this->_cbinstream = cb;
}

// maybe this could be implemented by app and not by this 
void Socket::queueCallback(void * in) {
	LOG_DEBUG("> %s", __func__);
	Socket * skt = (Socket *) in;

	BFRetain(skt);

	while (!BFThreadAsyncIsCanceled(skt->_tidq)) {
		skt->_inq.lock();
		// if queue is not empty, send the next message
		if (!skt->_inq.unsafeget().empty()) {
			// get first message
			struct Socket::Envelope * envelope = skt->_inq.unsafeget().front();

			if (envelope) {
				skt->_cbinstream(envelope->sc, envelope->buf.data, envelope->buf.size);
			}

			// pop queue
			skt->_inq.unsafeget().pop();

			BFFree(envelope->buf.data);
			BFFree(envelope);
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
	SocketConnection * mainConnection;
	Socket * socket;
};

void Socket::inStream(void * in) {
	LOG_DEBUG("> %s", __func__);
	InStreamTools * tools = (InStreamTools *) in; // we own memory
	SocketConnection * sc = tools->mainConnection;
	Socket * skt = tools->socket;
	BFThreadAsyncID tid = BFThreadAsyncGetID();

	BFRetain(skt);

	sc->_isready = true;	
	while (!BFThreadAsyncIsCanceled(tid)) {
		// create buffer
		struct Socket::Envelope * envelope = (struct Socket::Envelope *) malloc(sizeof(struct Socket::Envelope));
		envelope->buf.data = malloc(skt->_bufferSize);
		envelope->sc = sc;

		// receive data from connections using buffer
		int err = sc->recvData(&envelope->buf);
        if (!err) {
			skt->_inq.get().push(envelope);
		} else {
			BFFree(envelope->buf.data);
			BFFree(envelope);
			break;
		}
	}

	BFRelease(skt);
	Delete(tools);

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
			// get top data
			struct Socket::Envelope * envelope = skt->_outq.unsafeget().front();
			//struct Socket::Buffer * buf = &envelope->buf;

			// pop data from queue
			skt->_outq.unsafeget().pop();

			envelope->sc->sendData(&envelope->buf);

			BFFree(envelope->buf.data);
			BFFree(envelope);
		}
		skt->_outq.unlock();
	}

	BFRelease(skt);
	LOG_DEBUG("< %s", __func__);
}

// called by subclasses whenever they get a new connection
int Socket::startInStreamForConnection(SocketConnection * sc) {
	if (!sc) return 1;

	InStreamTools * tools = new InStreamTools;
	tools->mainConnection = sc;
	tools->socket = this;

	BFThreadAsyncID tid = BFThreadAsync(Socket::inStream, (void *) tools);
	this->_tidin.get().add(tid);

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

	// shutdown connections
	this->_connections.lock();
	for (int i = 0; i < this->_connections.unsafeget().count(); i++) {
		this->_connections.unsafeget().objectAtIndex(i)->closeConnection();
		//shutdown(this->_connections.unsafeget().objectAtIndex(i)->descriptor(), SHUT_RDWR);
		//close(this->_connections.unsafeget().objectAtIndex(i)->descriptor());
	}
	this->_connections.unlock();

	// tell subclasses that they can stop too
	error = this->_stop();

	// stop our threads
	
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

