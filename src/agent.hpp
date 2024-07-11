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

class User;

namespace BF {
	namespace Net {
		class SocketEnvelope;
		class SocketConnection;
	}
}

/**
 * Represents the remote user
 *
 * There are two subclasses, AgentServer & AgentClient. Both represent remote users but 
 * AgentServer are agents that are on a running server mode and client mode for the other
 */
class Agent : public BF::Object {
public:
	/**
	 * finds agent that is in charge of the socket connection to handle the buffered 
	 * data that came through the socket connection
	 *
	 * sc : each agent should have this
	 * bub : copy data if you need to use after function returns
	 */
	static void packetReceive(BF::Net::SocketEnvelope * envelope);

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
	static int newConnection(BF::Net::SocketConnection * sc);

	/**
	 * creates new agent	
	 *
	 * sc : we do not own the socket connection object
	 *
	 * retain count on return is +2. Caller is responsible for releasing object after they are done
	 *
	 * returns NULL if there is an error
	 */
	static Agent * create(BF::Net::SocketConnection * sc);

	virtual ~Agent();

	/**
	 * starts a conversation with the user on the other end
	 * to get to know them
	 */
	virtual int start() = 0;

	/**
	 * sends pkt to the socket connection we are assigned to
	 *
	 * pkt : is copied
	 */
	int sendPacket(const Packet * pkt);

	/**
	 * broadcasts packet to every agent
	 */
	static int broadcast(const Packet * pkt);

	/**
	 * the remote user we are representing
	 *
	 * since an agent can represent more than one user,
	 * we require the caller to know what user they are looking 
	 * for
	 */
	virtual User * getremoteuser(uuid_t uuid) = 0;

	virtual void setremoteuser(User * user) = 0;
	
	virtual bool representsUserWithUUID(const uuid_t uuid) = 0;

protected:

	Agent();

	bool connectionIsReady();

	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	virtual void receivedPayloadTypeUserInfo(const Packet * pkt);

	/**
	 * returns agent list
	 */
	static BF::Atomic<BF::List<Agent *>> * agentlist();
	
	virtual void updateremoteuser(const PayloadUserInfo * info) = 0;

private:

	void receivedPayloadTypeMessage(const Packet * pkt);
	void receivedPayloadTypeRequestAvailableChatrooms(const Packet * pkt);
	void receivedPayloadTypeChatroomInfo(const Packet * pkt);
	void receivedPayloadTypeChatroomEnrollment(const Packet * pkt);
	virtual void receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt) = 0;
	void receivedPayloadTypeChatroomResignation(const Packet * pkt);
	void receivedPayloadTypeChatroomEnrollmentRequest(const Packet * pkt);

	/**
	 * required by agent servers and clients
	 *
	 * agent servers will forward the quit notification to the other agents.
	 *
	 * the agent server will remove itself from the agent list, thus releasing
	 * itself from memory.
	 *
	 * agent will also release the user memory from everywhere (even
	 * the user internals)
	 *
	 * Note when this function is called, retain count is 2. The `packetReceive`
	 * function retains the agent and releases it before returning from function.
	 * This ensures memory can be safely accessed and is appropriately cleaned up
	 */
	virtual void receivedPayloadTypeNotifyQuitApp(const Packet * pkt) = 0;

	/*
	 * socket connection
	 *
	 * the socket descriptor that is wrapped within this object
	 * represents to user on other end we are representing
	 */
	BF::Net::SocketConnection * _sc;
	
	/**
	 * returns null if no agent was found for connection
	 *
	 * caller does not own memory
	 */
	static Agent * getAgentForConnection(BF::Net::SocketConnection * sc);
};

#endif // AGENT_HPP

