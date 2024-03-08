/**
 * author: brando
 * date: 1/24/24
 */

#include "client.hpp"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include "log.hpp"
#include "connection.hpp"
#include <arpa/inet.h>

Client::Client() {
}

Client::~Client() {
}

const char Client::mode() const {
	return SOCKET_MODE_CLIENT;
}

void Client::init(void * in) {
	Client * c = (Client *) in;

	BFRetain(c);

	int sock = 0;	
	sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001); // use some unused port number
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int connectStatus = connect(
		sock,
		(struct sockaddr *) &servAddr,
		sizeof(servAddr)
	);

	int err = 0;
    if (connectStatus == -1) {
		err = errno;
        printf("Error... %d\n", err);
	}

	SocketConnection * sc = NULL;
	if (!err) {
		sc = new SocketConnection(sock, c);
	}

	if (!err) {
		if (c->_cbnewconn)
			err = c->_cbnewconn(sc);
	}

	if (!err) {
		c->_connections.get().add(sc);
		c->startInStreamForConnection(sc);
    }

	BFRelease(c);
}

int Client::_start() {
	LOG_WRITE("client start");
	BFThreadAsyncID tid = BFThreadAsync(Client::init, this);
	BFThreadAsyncDestroy(tid);
	
	int error = 0;
	return error;
}

int Client::_stop() {
	LOG_WRITE("client stop");
	return 0;
}

