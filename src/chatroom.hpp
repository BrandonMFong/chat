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

private:
	BF::Atomic<BF::List<Message *>> _conversation;
};

#endif // CHATROOM_HPP

