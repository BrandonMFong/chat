/**
 * author: Brando
 * date: 6/28/22
 */

#include <stdio.h>
#include <string.h>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>

#define ARGUMENT_SERVER "server"
#define ARGUMENT_CLIENT "client"

#define CHAT_MODE_SERVER 's'
#define CHAT_MODE_CLIENT 'c'

void Help(const char * toolname) {
	printf("usage: %s\n", toolname);
}

int ArgumentsRead(int argc, char * argv[], char * mode) {
	if (!argv || !mode) return -2;
	else if (argc < 2) return -2;

	bool modereqclient = false;
	bool modereqserver = false;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], ARGUMENT_SERVER)) {
			modereqserver = true;
		} else if (!strcmp(argv[i], ARGUMENT_CLIENT)) {
			modereqclient = true;
		}
	}

	if (modereqclient && modereqserver) {
		return -2;
	} else if (modereqclient) {
		*mode = CHAT_MODE_CLIENT;
	} else if (modereqserver) {
		*mode = CHAT_MODE_SERVER;
	}

	return 0;
}

int ServerRun();
int ClientRun();

int main(int argc, char * argv[]) {
	int result = 0;
	char mode = 0;

	result = ArgumentsRead(argc, argv, &mode);
	if (result) {
		Help(argv[0]);
	} else {
		if (mode == CHAT_MODE_SERVER) {
			result = ServerRun();
		} else if (mode == CHAT_MODE_CLIENT) {
			result = ClientRun();
		}
	}

	return result;
}

int ClientRun() {
	printf("client\n");
	int sockD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port
        = htons(9001); // use some unused port number
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus
        = connect(sockD, (struct sockaddr*)&servAddr,
                  sizeof(servAddr));

    if (connectStatus == -1) {
        printf("Error...\n");
    }

    else {
		int i = 0;
		while (i < 10) {
			char buf[255];
			recv(sockD, buf, sizeof(buf), 0);
			printf("recv: %s\n", buf);
			snprintf(buf, 255, "client %d", i);
			send(sockD, buf, sizeof(buf), 0);
			printf("send: %s\n", buf);
			sleep(1);
			i++;
		}
    }

	return 0;
}

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

