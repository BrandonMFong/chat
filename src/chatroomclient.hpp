/**
 * author: brando
 * date: 3/2/24
 */

#ifndef CHATROOM_CLIENT_HPP
#define CHATROOM_CLIENT_HPP

#include "chatroom.hpp"

class AgentClient;

class ChatroomClient : public Chatroom {
public:
	/**
	 * creates a client chatroom that the main user
	 * can join
	 */
	static int recordChatroom(const PayloadChatInfo * info, AgentClient * agent);

	virtual ~ChatroomClient();
private:
	ChatroomClient(AgentClient * a, const uuid_t chatroomuuid);
	int sendPacket(const Packet * pkt);

	AgentClient * _agent;
};

#endif // CHATROOM_CLIENT_HPP

