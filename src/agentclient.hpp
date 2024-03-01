/**
 * author: brando
 * date: 2/27/24
 */

#ifndef AGENT_CLIENT_HPP
#define AGENT_CLIENT_HPP

#include "agent.hpp"

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
	 */
	int requestChatroomListUpdate();

private:
	AgentClient();
	int start();
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	static void setmain(AgentClient * ac);
};

#endif // AGENT_CLIENT_HPP

