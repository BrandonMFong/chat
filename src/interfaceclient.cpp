/**
 * author: brando
 * date: 3/3/24
 */

#include "interfaceclient.hpp"
#include "agentclient.hpp"
#include "log.hpp"

InterfaceClient::InterfaceClient() : Interface() {

}

InterfaceClient::~InterfaceClient() {

}

int InterfaceClient::draw() {
	this->Interface::draw();

	// only create new windows if
	// we changed states
	if (this->currstate() != this->prevstate()) {
		switch (this->currstate()) {
		case kInterfaceStateLobby:
			LOG_DEBUG("requesting a list of chatrooms we can join");
			// ask server for list of chats
			AgentClient::getmain()->requestChatroomListUpdate(this->getuser());
			break;
		case kInterfaceStateChatroom:
			break;
		case kInterfaceStateDraft:
			break;
		case kInterfaceStateHelp:
			break;
		}
	}

	return 0;
}


