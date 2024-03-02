/**
 * author: brando
 * date: 3/1/24
 */

#ifndef CHATROOM_SERVER_HPP
#define CHATROOM_SERVER_HPP

#include "chatroom.hpp"

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

private:
	ChatroomServer();
};

#endif // CHATROOM_SERVER_HPP

