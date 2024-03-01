/**
 * author: brando
 * date: 2/26/24
 */

#ifndef AGENT_HPP
#define AGENT_HPP

#include <bflibcpp/object.hpp>
#include "typepacket.h"

class SocketConnection;
class User;

/**
 * Represents the remote user
 *
 * There are two subclasses, AgentServer & AgentClient. Both represent remote users but 
 * AgentServer are agents that are on a running server mode and client mode for the other
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
	 * creates new agent	
	 *
	 * sc : we do not own the socket connection object
	 *
	 * retain count on return is +2. Caller is responsible for releasing object after they are done
	 */
	static Agent * create(SocketConnection * sc);

	virtual ~Agent();

	/**
	 * starts a conversation with the user on the other end
	 * to get to know them
	 */
	virtual int start() = 0;

protected:

	Agent();

	/*
	 * socket connection
	 *
	 * the socket descriptor that is wrapped within this object
	 * represents to user on other end we are representing
	 */
	SocketConnection * _sc;
	
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	void receivedPayloadTypeMessage(const Packet * pkt);
	void receivedPayloadTypeUserInfo(const Packet * pkt);

private:
	/**
	 * returns null if no agent was found for connection
	 *
	 * caller does not own memory
	 */
	static Agent * getAgentForConnection(SocketConnection * sc);

	/**
	 * the remote user we represent
	 */
	User * _remoteuser;
};

#endif // AGENT_HPP

