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
#include "chatdirectory.hpp"
#include "client.hpp"
#include "interface.hpp"
#include "log.hpp"
#include "user.hpp"
#include "office.hpp"
#include "agent.hpp"
#include <bflibcpp/bflibcpp.hpp>

#define ARGUMENT_SERVER "server"
#define ARGUMENT_CLIENT "client"

LOG_INIT

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
		*mode = SOCKET_MODE_CLIENT;
	} else if (modereqserver) {
		*mode = SOCKET_MODE_SERVER;
	}

	return 0;
}

int main(int argc, char * argv[]) {
	int result = 0;
	char mode = 0;
	Socket * skt = NULL;

	result = ArgumentsRead(argc, argv, &mode);

	if (mode == SOCKET_MODE_CLIENT)
		LOG_OPEN;

	LOG_DEBUG("============ App started ============");

	if (!result) {
		skt = Socket::create(mode, &result);

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
		result = InterfaceRun();
	}

	if (!result) {
		result = skt->stop();
	}

	if (result) {
		Help(argv[0]);
	}

	BFRelease(skt);

	LOG_DEBUG("============ App ended ============");
	LOG_CLOSE;

	return result;
}

