/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <server.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include <log.hpp>

Server::Server() : Socket() {
	this->_mainSocket = 0;
	this->_clientSocket = 0;
}

Server::~Server() {

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
	const int allowedConnections = 1;
    listen(s->_mainSocket, allowedConnections);

	s->_clientSocket = accept(s->_mainSocket, NULL, NULL);

	s->startIOStreams();
	
	BFRelease(s);
}

const int Server::descriptor() const {
	return this->_clientSocket;
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
	shutdown(this->_clientSocket, SHUT_RDWR);
	close(this->_clientSocket);
	close(this->_mainSocket);
	return 0;
}

