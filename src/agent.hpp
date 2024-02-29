/**
 * author: brando
 * date: 2/26/24
 */

#ifndef AGENT_HPP
#define AGENT_HPP

#include <bflibcpp/object.hpp>
#include "typepacket.h"

class SocketConnection;

/**
 * Represents the remote user
 *
 * mem management : retain/release
 */
class Agent : public BF::Object {
public:
	static void packetReceive(SocketConnection * sc, const void * buf, size_t size);

	/**
	 * this is a callback described by the Socket family
	 *
	 * this will get called when a new connection is made
	 * by the socket. 
	 *
	 * in this function we will create an agent that represents
	 * the conversation between us and the user on the other end.
	 *
	 * Once created, the agent will get to know the user on ther
	 * other end and get them ready to join a conversation.
	 */
	static int newConnection(SocketConnection * sc);

	/**
	 * creates new agent.  we do not own agent.
	 *
	 * sc : we do not own the socket connection object
	 */
	static Agent * create(SocketConnection * sc);

	virtual ~Agent();

	/**
	 * starts a conversation with the user on the other end
	 * to get to know them
	 */
	virtual int start() = 0;

protected:

	/**
	 * returns null if no agent was found for connection
	 *
	 * caller does not own memory
	 */
	static Agent * getAgentForConnection(SocketConnection * sc);

	void receivedPayloadTypeRequestInfo(const Packet * pkt);
	void receivedPayloadTypeMessage(const Packet * pkt);

	Agent();

	/*
	 * socket connection
	 *
	 * the socket descriptor that is wrapped within this object
	 * represents to user on other end we are representing
	 */
	SocketConnection * _sc;
};

#endif // AGENT_HPP

