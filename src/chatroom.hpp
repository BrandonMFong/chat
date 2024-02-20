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

class Chatroom : public BF::Object {
public:
	Chatroom();
	virtual ~Chatroom();

	/**
	 * adds message and flags an update
	 */
	int addMessage(const Message * msg);

	BF::Atomic<BF::List<Message *>> conversation;
	BF::Atomic<bool> updateConversation;

private:
};

#endif // CHATROOM_HPP

