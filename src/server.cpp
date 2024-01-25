/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <server.hpp>
#include <messenger.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

typedef struct {
	int cd; // client socket descriptor
	ChatConfig * config;
} ServerThreadTools;

void ServerThreadCallbackMessageIn(void * in) {
	ServerThreadTools * tools = (ServerThreadTools *) in;
	
	int i = 0;
	while (i < 10) {
		char buf[MESSAGE_BUFFER_SIZE];
        recv(tools->cd, buf, sizeof(buf), 0);
		printf("recv: %s\n", buf);
		sleep(1);
		i++;
	}
}

void ServerThreadCallbackMessageOut(void * in) {
	ServerThreadTools * tools = (ServerThreadTools *) in;

	int i = 0;
	while (i < 10) {
		char buf[MESSAGE_BUFFER_SIZE];
		snprintf(buf, MESSAGE_BUFFER_SIZE, "server %d", i);
		// send's messages to client socket
		send(tools->cd, buf, sizeof(buf), 0);
		printf("send: %s\n", buf);
		sleep(1);
		i++;
	}
}

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

	ServerThreadTools tools[allowedConnections];

    // integer to hold client socket.
    tools[0].cd = accept(serverSocket, NULL, NULL);
	tools[0].config = config;

	//BFThreadAsyncID tid0 = BFThreadAsync(ServerThreadCallbackMessageIn, (void *) &tools[0]);
	BFThreadAsyncID tid1 = BFThreadAsync(ServerThreadCallbackMessageOut, (void *) &tools[0]);

	while (1) {}
}

int ServerRun(ChatConfig * config) {
	printf("server\n");
	BFThreadAsyncID tid = BFThreadAsync(ServerThreadCallbackInit, (void *) config);
	return MessengerRun(config);
}

