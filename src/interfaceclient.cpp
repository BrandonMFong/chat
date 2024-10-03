/**
 * author: brando
 * date: 3/3/24
 */

#include "interfaceclient.hpp"
#include "agentclient.hpp"
#include "log.hpp"
#include "chatroom.hpp"

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
			Chatroom::clearChatroomList();

			// ask server for list of chats
			AgentClient::getmain()->requestChatroomListUpdate(this->getuser());
			break;
		case kInterfaceStateChatroom:
			break;
		case kInterfaceStateDraft:
			break;
		case kInterfaceStateHelp:
			break;
		default:
			break;
		}
	}

	return 0;
}


