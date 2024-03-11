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

private:
	AgentClient();
	int start();
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	static void setmain(AgentClient * ac);

	virtual void receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt);
	virtual void requestPayloadTypeNotifyQuitApp(const Packet * pkt);
};

#endif // AGENT_CLIENT_HPP

