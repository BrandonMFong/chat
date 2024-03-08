/**
 * author: brando
 * date: 2/27/24
 */

#ifndef AGENT_CLIENT_HPP
#define AGENT_CLIENT_HPP

#include "agent.hpp"

class User;

/**
 * represents remote user in client mode
 *
 * there should only be one AgentClient on the client side
 */
class AgentClient : public Agent {
	friend class Agent;
public:
	virtual ~AgentClient();

	static AgentClient * getmain();

	/**
	 * Asks the server for updated list of
	 * chatrooms
	 *
	 * user : the user that is requesting data. This is
	 * the main user on the machine
	 */
	int requestChatroomListUpdate(const User * user);

	/**
	 * tells server we are joining the chatroom with uuid
	 *
	 * this gives the server a notification that there is a new
	 * recipient that should receive chatroom messages
	 *
	 * we may want to have an approval step after this but for
	 * now we are just joining without it
	 */
	//int enrollInChatroom(const PayloadChatInfo * chatinfo);

private:
	AgentClient();
	int start();
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	static void setmain(AgentClient * ac);
};

#endif // AGENT_CLIENT_HPP

