/**
 * author: Brando
 * date: 6/28/22
 *
 * https://www.geeksforgeeks.org/simple-client-server-application-in-c/
 */

#include "chat.hpp"
#include <stdio.h>
#include <string.h>
#include "interface.hpp"
#include "log.hpp"
#include "user.hpp"
#include "office.hpp"
#include "agent.hpp"
#include "version.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include <bfnet/bfnet.hpp>
#include <netinet/ip.h>

#define ARGUMENT_SERVER "server"
#define ARGUMENT_CLIENT "client"
#define ARGUMENT_IP4_ADDRESS "-ip4"
#define ARGUMENT_VERSION "--version"
#define ARGUMENT_HELP "--help"

#define PRINTF_ERR(...) printf("ERROR - " __VA_ARGS__)

using namespace BF;
using namespace BF::Net;

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
	printf("  [ %s ]\tReturns version\n", ARGUMENT_VERSION);
	printf("  [ %s ]\tShows help\n", ARGUMENT_HELP);

	printf("\nCopyright © 2024 Brando. All rights reserved.\n");
}

int ArgumentsRead(
	int argc, char * argv[],
	char * mode, char * ipaddr,
	bool * showvers, bool * showhelp
) {
	if (!argv || !mode || !ipaddr 
		|| !showvers || !showhelp) return 2;
	else if (argc < 1) return 3;

	bool modereqclient = false;
	bool modereqserver = false;
	bool ip4addrpassed = false;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], ARGUMENT_SERVER)) {
			modereqserver = true;
		} else if (!strcmp(argv[i], ARGUMENT_CLIENT)) {
			modereqclient = true;
		} else if (!strcmp(argv[i], ARGUMENT_IP4_ADDRESS)) {
			ip4addrpassed = true;
			strncpy(ipaddr, argv[++i], SOCKET_IP4_ADDR_STRLEN);
		} else if (!strcmp(argv[i], ARGUMENT_VERSION)) {
			*showvers = true;
		} else if (!strcmp(argv[i], ARGUMENT_HELP)) {
			*showhelp = true;
		}
	}

	if (!(*showvers) && !(*showhelp)) {
		// make sure server/client aren't both passed
		if (modereqclient && modereqserver) {
			PRINTF_ERR("cannot request to be both server and client\n");
			return 4;
		} else if (modereqclient) {
			*mode = SOCKET_MODE_CLIENT;
		} else if (modereqserver) {
			*mode = SOCKET_MODE_SERVER;
		}

		if (*mode == SOCKET_MODE_CLIENT) {
			if (!ip4addrpassed) {
				PRINTF_ERR("please provided an ip address of the server you want to join\n");
				return 5;
			}
		}
	}

	return 0;
}

const char Chat::SocketGetMode() {
	if (skt) return skt->mode();
	return ' ';
}

int _ChatRun(char mode, const char * ipaddr) {
	int result = 0;
	Interface * interface = NULL;

	if (!result) {
		result = Office::start();
	}

	if (!result) {
		interface = Interface::create(mode);
	}

	if (!result) {
		skt = Socket::create(mode, ipaddr, CHAT_SOCKET_SERVER_PORT_NUM, &result);

		if (skt) {
			skt->setInStreamCallback(Office::packetReceive);
			skt->setNewConnectionCallback(Agent::newConnection);
			skt->setBufferSize(CHAT_SOCKET_BUFFER_SIZE);
		}
	}

	if (!result) {
		result = skt->start();
	}

	// this will start user interaction
	if (!result) {
		result = interface->run();
	}

	if (!result) {
		result = skt->stop();
	}

	if (!result) {
		result = Office::stop();
	}

	BFRelease(skt);
	BFRelease(interface);

	return result;
}

void _ChatShowVersion(const char * toolname) {
	printf("%s version %s\n", toolname, VERSION_WHOLE_STRING);
}

int Chat::Main(int argc, char * argv[]) {
	int result = 0;
	char mode = SOCKET_MODE_CLIENT;
	bool showversion = false;
	bool showhelp = false;
	Interface * interface = NULL;
	char ipaddr[SOCKET_IP4_ADDR_STRLEN];

	// default ip addr is localhost
	strncpy(ipaddr, "0.0.0.0", SOCKET_IP4_ADDR_STRLEN);

	result = ArgumentsRead(argc, argv, &mode, ipaddr, &showversion, &showhelp);

	LOG_OPEN;

	LOG_DEBUG("============ App started ============");

	if (showversion) {
		_ChatShowVersion(argv[0]);
	} else if (showhelp) {
		Help(argv[0]);
	} else {
		if (!result) {
			result = _ChatRun(mode, ipaddr);
		}

		if (result) {
			Help(argv[0]);
		}
	}

	LOG_DEBUG("============ App ended ============");
	LOG_CLOSE;

	return result;
}

