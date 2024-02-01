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
	Server * server = (Server *) in;

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

	BFThreadAsync(Socket::inStream, (void *) server);
	BFThreadAsync(Socket::outStream, (void *) server);

	while (1) {}
}

const int Server::descriptor() const {
	return this->_clientSocket;
}

int Server::start() {
	printf("server\n");

	BFThreadAsync(Server::init, (void *) this);

	int error = 0;

	return error;
}

int Server::stop() {
	return 0;
}

