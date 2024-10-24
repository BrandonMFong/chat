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
	virtual void setremoteuser(User * user);
	virtual User * getremoteuser(uuid_t uuid);

private:
	AgentServer();

	int start();

	/**
	 * waits for the socket connection to be ready before start
	 * a conversation with the remote user
	 */
	static void handshake(void * in);

	virtual void receivedPayloadTypeNotifyChatroomListChanged(const Packet * pkt);
	virtual void receivedPayloadTypeNotifyQuitApp(const Packet * pkt);
	virtual void receivedPayloadTypeRequestInfo(const Packet * pkt);
	virtual void receivedPayloadTypeUserInfo(const Packet * pkt);

	virtual bool representsUserWithUUID(const uuid_t uuid);
	virtual void updateremoteuser(const PayloadUserInfo * info);

	int forwardPacket(const Packet * pkt);

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

#endif // AGENT_SERVER_HPP

