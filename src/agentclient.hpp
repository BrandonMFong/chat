/**
 * author: brando
 * date: 2/27/24
 */

#ifndef AGENT_CLIENT_HPP
#define AGENT_CLIENT_HPP

#include "agent.hpp"

/**
 * represents remote user in client mode
 */
class AgentClient : public Agent {
	friend class Agent;
public:
	virtual ~AgentClient();

private:
	AgentClient();
	int start();
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
};

#endif // AGENT_CLIENT_HPP

