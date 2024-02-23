/**
 * author: brando
 * date: 1/24/24
 */

#include "chat.h"
#include "server.hpp"
#include "investigate.hpp"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"

Server::Server() : Socket() {
	this->_mainSocket = 0;
	this->_pollt = NULL;
}

Server::~Server() {
	BFThreadAsyncDestroy(this->_pollt);
}

const char Server::mode() const {
	return SOCKET_MODE_SERVER;
}

void Server::init(void * in) {
	Server * s = (Server *) in;

	BFRetain(s);

	// create server socket similar to what was done in
    // client program
    s->_mainSocket = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to the specified IP and port
    bind(s->_mainSocket, (struct sockaddr *) &servAddr, sizeof(servAddr));

    // listen for connections
    listen(s->_mainSocket, CHAT_ROOM_MAX_SIZE);

	// launch thread that will constantly accept multiple connections
	s->_pollt = BFThreadAsync(Server::pollthread, s);

	BFRelease(s);
}

void Server::pollthread(void * in) {
	Server * s = (Server *) in;
	BFRetain(s);

	while (!BFThreadAsyncIsCanceled(s->_pollt)) {
		int csock = accept(s->_mainSocket, NULL, NULL);
		LOG_DEBUG("new connection: %d", csock);

		int err = Investigate::NewConnection(csock);

		// if there are no errors with connection then
		// we will add it to our connections list
		if (!err) {
			s->_connections.get().add(csock);
			s->startInStreamForConnection(csock);
		}
	}

	BFRelease(s);
}

int Server::_start() {
	LOG_WRITE("server start");

	BFThreadAsyncID tid = BFThreadAsync(Server::init, this);
	BFThreadAsyncDestroy(tid);
	
	int error = 0;

	return error;
}

int Server::_stop() {
	LOG_WRITE("server stop");
	shutdown(this->_mainSocket, SHUT_RDWR);
	close(this->_mainSocket);

	// thread break down
	BFThreadAsyncCancel(this->_pollt);
	BFThreadAsyncWait(this->_pollt);

	return 0;
}

