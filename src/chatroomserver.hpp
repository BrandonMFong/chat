/**
 * author: brando
 * date: 3/1/24
 */

#ifndef CHATROOM_SERVER_HPP
#define CHATROOM_SERVER_HPP

#include "chatroom.hpp"
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/list.hpp>

class AgentServer;

/**
 */
class ChatroomServer : public Chatroom {
public:
	/**
	 * creates a chatroom
	 *
	 * name : chat room name
	 *
	 * retain count on return is +2. Caller is responsible for releasing object after they are done
	 */
	static ChatroomServer * create(const char * name);

	virtual ~ChatroomServer();

	void addAgent(AgentServer * a);

private:
	ChatroomServer();
	int sendPacket(const Packet * pkt);

	BF::Atomic<BF::List<AgentServer *>> _agents;
};

#endif // CHATROOM_SERVER_HPP

