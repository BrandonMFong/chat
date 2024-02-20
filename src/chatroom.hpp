/**
 * author: brando
 * date: 2/20/24
 */

#ifndef CHATROOM_HPP
#define CHATROOM_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/list.hpp>
#include "typemessage.h"

#define CHAT_ROOM_NAME_SIZE 255

class Chatroom : public BF::Object {
public:
	Chatroom();
	virtual ~Chatroom();

	/**
	 * adds message and flags an update
	 */
	int addMessage(const Message * msg);

	void setName(const char * name);

	BF::Atomic<BF::List<Message *>> conversation;
	BF::Atomic<bool> updateConversation;

private:
	char _uuid[kBFStringUUIDStringLength];
	char _name[CHAT_ROOM_NAME_SIZE];
};

#endif // CHATROOM_HPP

