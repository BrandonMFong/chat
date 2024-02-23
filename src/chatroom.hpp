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
 */
class Chatroom : public BF::Object {
public:
	/**
	 * inits chatroom with uuid
	 */
	Chatroom(const char * uuid);
	virtual ~Chatroom();

	/**
	 * adds message and flags an update
	 *
	 * msg : memory must be created before calling
	 */
	int addMessage(Message * msg);

	void setName(const char * name);

	int sendBuffer(const InputBuffer * buf);

	const char * uuid();

	BF::Atomic<BF::List<Message *>> conversation;
	BF::Atomic<bool> updateConversation;

private:
	char _uuid[kBFStringUUIDStringLength];
	char _name[CHAT_ROOM_NAME_SIZE];
};

#endif // CHATROOM_HPP

