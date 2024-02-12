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
	this->_initthreadid = 0;
}

Server::~Server() {

}

const char Server::mode() const {
	return SOCKET_MODE_SERVER;
}

void Server::init(void * in) {
	Server * server = (Server *) in;

	BFRetain(server);

	// create server socket similar to what was done in
    // client program
    server->_mainSocket = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to the specified IP and port
    bind(server->_mainSocket, (struct sockaddr *) &servAddr, sizeof(servAddr));

    // listen for connections
	const int allowedConnections = 1;
    listen(server->_mainSocket, allowedConnections);

	server->_clientSocket = accept(server->_mainSocket, NULL, NULL);

	server->startIOStreams();

	BFRelease(server);
}

const int Server::descriptor() const {
	return this->_clientSocket;
}

int Server::_start() {
	LOG_WRITE("server start");

	this->_initthreadid = BFThreadAsync(Server::init, (void *) this);

	int error = 0;

	return error;
}

int Server::_stop() {
	LOG_WRITE("server stop");
	BFThreadAsyncCancel(this->_initthreadid);
	BFThreadAsyncIDDestroy(this->_initthreadid);
	shutdown(this->_clientSocket, SHUT_RDWR);
	close(this->_clientSocket);
	close(this->_mainSocket);
	return 0;
}

