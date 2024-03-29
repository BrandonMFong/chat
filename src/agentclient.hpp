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

	virtual void setremoteuser(User * user);
	
	virtual User * getremoteuser(uuid_t uuid);

private:
	AgentClient();
	int start();
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	static void setmain(AgentClient * ac);

	virtual void receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt);
	virtual void receivedPayloadTypeNotifyQuitApp(const Packet * pkt);
	virtual bool representsUserWithUUID(const uuid_t uuid);
	virtual void updateremoteuser(const PayloadUserInfo * info);

	/**
	 * an agent on the client end can represent many remote users
	 *
	 * this is because this agent's socket connection is to the server 
	 * and the server has many users connected to it.
	 */
	BF::Atomic<BF::List<User *>> _remoteusers;
};

#endif // AGENT_CLIENT_HPP

