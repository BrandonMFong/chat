/**
 * author: Brando
 * date: 6/28/22
 *
 * https://www.geeksforgeeks.org/simple-client-server-application-in-c/
 */

#include "chat.hpp"
#include <stdio.h>
#include <string.h>
#include "server.hpp"
#include "client.hpp"
#include "interface.hpp"
#include "log.hpp"
#include "user.hpp"
#include "office.hpp"
#include "agent.hpp"
#include <bflibcpp/bflibcpp.hpp>

#define ARGUMENT_SERVER "server"
#define ARGUMENT_CLIENT "client"
#define ARGUMENT_IP4_ADDRESS "-ip4"

LOG_INIT
	
Socket * skt = NULL;

void Help(const char * toolname) {
	printf("usage: %s <mode> [ %s <ip4 address> ]\n", toolname, ARGUMENT_IP4_ADDRESS);
	printf("\n");
	printf("Arguments:\n");
	printf("  <mode>\tEither 'server' or 'client'. Server will mean hosting your\n");
	printf("\t\town chat server with admin privileges. Client will mean you\n");
	printf("\t\twill be a participant in a chatroom.  Additionally, client mode\n");
	printf("\t\tis implied so you don't have to pass `client`.\n");
	printf("  [ %s ]\tThe server's ip4 address. Server mode does not require this\n", ARGUMENT_IP4_ADDRESS);

	printf("\nCopyright © 2024 Brando. All rights reserved.\n");
}

int ArgumentsRead(int argc, char * argv[], char * mode, char * ipaddr) {
	if (!argv || !mode || !ipaddr) return 2;
	else if (argc < 1) return 3;

	bool modereqclient = false;
	bool modereqserver = false;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], ARGUMENT_SERVER)) {
			modereqserver = true;
		} else if (!strcmp(argv[i], ARGUMENT_CLIENT)) {
			modereqclient = true;
		} else if (!strcmp(argv[i], ARGUMENT_IP4_ADDRESS)) {
			strncpy(ipaddr, argv[i], SOCKET_IP4_ADDR_STRLEN);
		}
	}

	if (modereqclient && modereqserver) {
		printf("cannot request to be both server and client\n");
		return 4;
	} else if (modereqclient) {
		*mode = SOCKET_MODE_CLIENT;
	} else if (modereqserver) {
		*mode = SOCKET_MODE_SERVER;
	}

	return 0;
}

const char ChatSocketGetMode() {
	if (skt) return skt->mode();
	return ' ';
}

int main(int argc, char * argv[]) {
	int result = 0;
	char mode = SOCKET_MODE_CLIENT;
	Interface * interface = NULL;
	char ipaddr[SOCKET_IP4_ADDR_STRLEN];

	// default ip addr is localhost
	strncpy(ipaddr, "127.0.0.1", SOCKET_IP4_ADDR_STRLEN);

	result = ArgumentsRead(argc, argv, &mode, ipaddr);

	LOG_OPEN;

	LOG_DEBUG("============ App started ============");

	if (!result) {
		skt = Socket::create(mode, ipaddr, CHAT_SOCKET_SERVER_PORT_NUM, &result);

		if (skt) {
			skt->setInStreamCallback(Agent::packetReceive);
			skt->setNewConnectionCallback(Agent::newConnection);
			skt->setBufferSize(sizeof(Packet));
		}
	}

	if (!result) {
		result = skt->start();
	}

	if (!result) {
		interface = Interface::create(mode);
	}

	if (!result) {
		result = interface->run();
	}

	if (!result) {
		result = skt->stop();
	}

	if (result) {
		Help(argv[0]);
	}

	BFRelease(skt);
	BFRelease(interface);

	LOG_DEBUG("============ App ended ============");
	LOG_CLOSE;

	return result;
}

