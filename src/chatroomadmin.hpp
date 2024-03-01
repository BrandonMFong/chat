/**
 * author: brando
 * date: 3/1/24
 */

#ifndef CHATROOM_ADMIN_HPP
#define CHATROOM_ADMIN_HPP

#include "chatroom.hpp"

/**
 * Extends the chatroom class for
 * privileged access like modification, creation, etc.
 *
 * I made this to isolate the part of the code that 
 * creates chatrooms.  We should only be able to 
 * create chatrooms if we are running in server mode
 * `ChatSocketGetMode()`
 */
class ChatroomAdmin : public Chatroom {
public:
	/**
	 * creates a chatroom
	 *
	 * name : chat room name
	 *
	 * retain count on return is +2. Caller is responsible for releasing object after they are done
	 */
	static ChatroomAdmin * create(const char * name);

	virtual ~ChatroomAdmin();

private:
	ChatroomAdmin();
};

#endif // CHATROOM_ADMIN_HPP

