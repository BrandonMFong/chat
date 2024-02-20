/**
 * author: brando
 * date: 2/20/24
 */

#ifndef CHAT_DIRECTORY_HPP
#define CHAT_DIRECTORY_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/list.hpp>

class Chatroom;

class ChatDirectory : public BF::Object {
public:
	static ChatDirectory * shared();
	ChatDirectory();
	virtual ~ChatDirectory();

	Chatroom * getChatroom();
	void addChatroom(Chatroom * room);

private:
	BF::Atomic<BF::List<Chatroom *>> _chatrooms;
};

#endif // CHAT_DIRECTORY_HPP

