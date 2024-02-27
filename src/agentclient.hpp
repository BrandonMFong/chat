/**
 * author: brando
 * date: 2/27/24
 */

#ifndef AGENT_CLIENT_HPP
#define AGENT_CLIENT_HPP

#include "agent.hpp"

class AgentClient : public Agent {
	friend class Agent;
public:
	virtual ~AgentClient();

private:
	AgentClient();
};

#endif // AGENT_CLIENT_HPP

