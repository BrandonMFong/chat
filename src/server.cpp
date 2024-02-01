/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <server.hpp>
#include <io.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

Server::Server() {

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
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to the specified IP and port
    bind(serverSocket, (struct sockaddr *) &servAddr, sizeof(servAddr));

    // listen for connections
	const int allowedConnections = 1;
    listen(serverSocket, allowedConnections);

	server->sockd = accept(serverSocket, NULL, NULL);

	BFThreadAsync(Socket::inStream, (void *) server);
	BFThreadAsync(Socket::outStream, (void *) server);

	while (1) {}
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

