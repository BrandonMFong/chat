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

	static Agent * getmain();

private:
	AgentClient();
	int start();
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
};

#endif // AGENT_CLIENT_HPP

