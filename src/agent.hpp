/**
 * author: brando
 * date: 2/26/24
 */

#ifndef AGENT_HPP
#define AGENT_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/list.hpp>
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
	 *
	 * returns NULL if there is an error
	 */
	static Agent * create(SocketConnection * sc);

	virtual ~Agent();

	/**
	 * starts a conversation with the user on the other end
	 * to get to know them
	 */
	virtual int start() = 0;

	/**
	 * pkt : is copied
	 */
	int sendPacket(const Packet * pkt);

	/**
	 * broadcasts packet to every agent
	 */
	static int broadcast(const Packet * pkt);

	/**
	 * the remote user we are representing
	 */
	User * user();

protected:

	Agent();

	bool connectionIsReady();

	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	virtual void receivedPayloadTypeUserInfo(const Packet * pkt);

	/**
	 * returns agent list
	 */
	static BF::Atomic<BF::List<Agent *>> * agentlist();

private:

	void receivedPayloadTypeMessage(const Packet * pkt);
	void receivedPayloadTypeRequestAvailableChatrooms(const Packet * pkt);
	void receivedPayloadTypeChatroomInfo(const Packet * pkt);
	void receivedPayloadTypeChatroomEnrollment(const Packet * pkt);
	virtual void receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt) = 0;
	void requestPayloadTypeChatroomResignation(const Packet * pkt);
	
	/**
	 * required by agent servers and clients
	 *
	 * agent servers will forward the quit notification to the other agents.
	 *
	 * the agent server will remove itself from the agent list, thus releasing
	 * itself from memory.
	 *
	 * Note when this function is called, retain count is 2. The `packetReceive`
	 * function retains the agent and releases it before returning from function.
	 * This ensures memory can be safely accessed and is appropriately cleaned up
	 */
	virtual void requestPayloadTypeNotifyQuitApp(const Packet * pkt) = 0;

	/*
	 * socket connection
	 *
	 * the socket descriptor that is wrapped within this object
	 * represents to user on other end we are representing
	 */
	SocketConnection * _sc;
	
	/**
	 * returns null if no agent was found for connection
	 *
	 * caller does not own memory
	 */
	static Agent * getAgentForConnection(SocketConnection * sc);

	/**
	 * the remote user we represent
	 *
	 * this doesn't provide a deep representation
	 * of the actual remote user. This object should
	 * hold enough information to work with
	 *
	 * See class header for more info
	 */
	User * _remoteuser;
};

#endif // AGENT_HPP

