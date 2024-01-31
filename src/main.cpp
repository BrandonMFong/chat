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
#include <typechatconfig.h>
#include <interface.hpp>

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

int main(int argc, char * argv[]) {
	int result = 0;
	char mode = 0;
	ChatConfig config;

	result = ArgumentsRead(argc, argv, &mode);
	if (!result) {
		if (mode == CHAT_MODE_SERVER) {
			result = ServerRun(&config);
		} else if (mode == CHAT_MODE_CLIENT) {
			result = ClientRun(&config);
		}
	}

	if (!result) {
		result = InterfaceRun(&config);
	}

	if (result) {
		Help(argv[0]);
	}

	return result;
}

