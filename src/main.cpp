/**
 * author: Brando
 * date: 6/28/22
 *
 * https://www.geeksforgeeks.org/simple-client-server-application-in-c/
 */

#include <stdio.h>
#include <string.h>
#include <server.hpp>
#include <client.hpp>
#include <interface.hpp>
#include <log.hpp>
#include <user.hpp>
#include <bflibcpp/bflibcpp.hpp>

LOG_INIT

#define ARGUMENT_SERVER "server"
#define ARGUMENT_CLIENT "client"

User * curruser = 0;

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

int InitializeUser() {
	curruser = new User;
	return 0;
}

int main(int argc, char * argv[]) {
	int result = 0;
	char mode = 0;
	Socket * skt = NULL;

	LOG_OPEN;

	LOG_DEBUG("============ App started ============");

	result = ArgumentsRead(argc, argv, &mode);
	if (!result) {
		skt = Socket::create(mode, &result);
	}

	if (!result) {
		result = InitializeUser();
	}

	if (!result) {
		skt->setQueueCallback(InterfaceInStreamQueueCallback);
		result = skt->start();
	}

	if (!result) {
		result = InterfaceRun(skt);
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

