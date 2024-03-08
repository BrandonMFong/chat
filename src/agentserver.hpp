/**
 * author: brando
 * date: 2/27/24
 */

#ifndef AGENT_SERVER_HPP
#define AGENT_SERVER_HPP

#include "agent.hpp"

/**
 * represents remote user in server mode
 */
class AgentServer : public Agent {
	friend class Agent;
public:
	virtual ~AgentServer();

private:
	AgentServer();

	int start();

	/**
	 * waits for the socket connection to be ready before start
	 * a conversation with the remote user
	 */
	static void handshake(void * in);

	virtual void receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt);
};

#endif // AGENT_SERVER_HPP

