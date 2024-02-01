/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <client.hpp>
#include <io.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

Client::Client() {

}

Client::~Client() {

}

const char Client::mode() const {
	return SOCKET_MODE_CLIENT;
}

void Client::init(void * in) {
	Client * client = (Client *) in;

	int sockD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001); // use some unused port number
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus = connect(
		sockD,
		(struct sockaddr *) &servAddr,
		sizeof(servAddr)
	);

    if (connectStatus == -1) {
        printf("Error... %d\n", errno);
    } else {
		client->sockd = sockD;
		BFThreadAsync(Socket::inStream, (void *) client);
		BFThreadAsync(Socket::outStream, (void *) client);

		while (1) {}
    }
}

int Client::start() {
	printf("client\n");
	BFThreadAsync(Client::init, (void *) this);

	int error = 0;
	return error;
}

int Client::stop() {
	return 0;
}

