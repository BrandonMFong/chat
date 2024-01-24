/**
 * author: brando
 * date: 1/24/24
 */

#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

int ServerRun() {
	printf("server\n");
	// create server socket similar to what was done in
    // client program
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to the specified IP and port
    bind(servSockD, (struct sockaddr*)&servAddr,
         sizeof(servAddr));

    // listen for connections
    listen(servSockD, 1);

    // integer to hold client socket.
    int clientSocket = accept(servSockD, NULL, NULL);

	int i = 0;
	while (i < 10) {
		char buf[255];
		snprintf(buf, 255, "server %d", i);
		// send's messages to client socket
		send(clientSocket, buf, sizeof(buf), 0);
		printf("send: %s\n", buf);
        recv(clientSocket, buf, sizeof(buf), 0);
		printf("recv: %s\n", buf);
		sleep(1);
		i++;
	}

	return 0;
}

