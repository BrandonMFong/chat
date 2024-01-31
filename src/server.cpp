/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <server.hpp>
#include <messenger.hpp>
#include <io.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

void ServerThreadCallbackInit(void * in) {
	ChatConfig * config = (ChatConfig *) in;

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

	IOTools iotools[allowedConnections];

    // integer to hold client socket.
	iotools[0].config = config;
	iotools[0].cd = accept(serverSocket, NULL, NULL);

	//BFThreadAsync(IOIn, (void *) &iotools[0]);
	BFThreadAsync(IOOut, (void *) &iotools[0]);

	while (1) {}
}

int ServerRun(ChatConfig * config) {
	printf("server\n");

	BFThreadAsyncID tid = BFThreadAsync(ServerThreadCallbackInit, (void *) config);

	int error = MessengerRun(config);

	return error;
}

