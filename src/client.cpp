/**
 * author: brando
 * date: 1/24/24
 */

#include <chat.h>
#include <client.hpp>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>
#include <log.hpp>

Client::Client() {
	this->_mainSocket = 0;
}

Client::~Client() {

}

const char Client::mode() const {
	return SOCKET_MODE_CLIENT;
}

void Client::init() {
	this->_mainSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001); // use some unused port number
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus = connect(
		this->_mainSocket,
		(struct sockaddr *) &servAddr,
		sizeof(servAddr)
	);

    if (connectStatus == -1) {
        printf("Error... %d\n", errno);
    } else {
		this->startIOStreams();
    }
}

const int Client::descriptor() const {
	return this->_mainSocket;
}

int Client::_start() {
	LOG_WRITE("client start");
	this->init();

	int error = 0;
	return error;
}

int Client::_stop() {
	LOG_WRITE("client stop");
	shutdown(this->_mainSocket, SHUT_RDWR);
	close(this->_mainSocket);
	return 0;
}

