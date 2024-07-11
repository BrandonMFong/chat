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

	/**
	 * +1 to agent's retain count
	 */
	//virtual int addAgent(Agent * a);

	/**
	 * this overrides `Chatroom`'s implementation by adding a step 
	 * to act as a proxy and send incoming packet
	 * to all our agents excluding the packet sender
	 */
	virtual int receiveMessagePacket(const Packet * pkt);

private:
	ChatroomServer();
	int sendPacket(const Packet * pkt);
	int initCipher();
	int requestEnrollment(User * user);
};

#endif // CHATROOM_SERVER_HPP

