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
	 *
	 * agent : the agent that reps the user who created it
	 */
	static int recordChatroom(const PayloadChatInfo * info, Agent * agent);

	virtual ~ChatroomClient();

private:
	AgentClient * agent();
	ChatroomClient(const uuid_t chatroomuuid, AgentClient * agent);
	int sendPacket(const Packet * pkt);
};

#endif // CHATROOM_CLIENT_HPP

