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
} ServerThreadIOTools;

void ServerThreadCallbackMessageIn(void * in) {
	ServerThreadIOTools * tools = (ServerThreadIOTools *) in;
	
	int i = 0;
	while (i < 10) {
		char buf[MESSAGE_BUFFER_SIZE];
        recv(tools->cd, buf, sizeof(buf), 0);
		i++;
	}
}

void ServerThreadCallbackMessageOut(void * in) {
	ServerThreadIOTools * tools = (ServerThreadIOTools *) in;

	while (1) {
		tools->config->out.lock();
		// if queue is not empty, send the next message
		if (!tools->config->out.get().empty()) {
			// get first message
			Message * msg = tools->config->out.get().front();

			// pop queue
			tools->config->out.get().pop();

			// send buf from message
			send(tools->cd, msg->buf, sizeof(msg->buf), 0);

			MESSAGE_FREE(msg);
		}
		tools->config->out.unlock();
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

	ServerThreadIOTools iotools[allowedConnections];

    // integer to hold client socket.
	iotools[0].config = config;
	iotools[0].cd = accept(serverSocket, NULL, NULL);

	//BFThreadAsyncID inid = BFThreadAsync(ServerThreadCallbackMessageIn, (void *) &iotools[0]);
	BFThreadAsyncID outid = BFThreadAsync(ServerThreadCallbackMessageOut, (void *) &iotools[0]);

	while (1) {}

	//BFThreadAsyncIDDestroy(inid);
	BFThreadAsyncIDDestroy(outid);
}

int ServerRun(ChatConfig * config) {
	printf("server\n");

	BFThreadAsyncID tid = BFThreadAsync(ServerThreadCallbackInit, (void *) config);

	int error = MessengerRun(config);

	BFThreadAsyncIDDestroy(tid);

	return error;
}

