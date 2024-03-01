/**
 * author: brando
 * date: 2/20/24
 */

#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/list.hpp>
#include "inputbuffer.hpp"

extern "C" {
#include <bflibc/stringutils.h>
}

#define CHAT_ROOM_NAME_SIZE 255

class Message;

/**
 * In charge of creating message
 *
 * This class has its own memory management
 */
class Chatroom : public BF::Object {
public:
	/**
	 * creates a chatroom
	 *
	 * name : chat room name
	 *
	 * retain count on return is +2. Caller is responsible for releasing object after they are done
	 */
	static Chatroom * create(const char * name);

	/// returns chat room for uuid
	static Chatroom * getChatroom(uuid_t chatroomuuid);

	virtual ~Chatroom();

	/**
	 * adds message and flags an update
	 *
	 * msg : memory must be created before calling
	 */
	int addMessage(Message * msg);

	int sendBuffer(const InputBuffer * buf);

	void getuuid(uuid_t uuid);

	BF::Atomic<BF::List<Message *>> conversation;
	BF::Atomic<bool> updateConversation;

private:

	/**
	 * inits chatroom with uuid
	 */
	Chatroom();

	uuid_t _uuid;
	char _name[CHAT_ROOM_NAME_SIZE];
};

#endif // CHATROOM_HPP

