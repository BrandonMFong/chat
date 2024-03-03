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
#include "typepacket.h"

extern "C" {
#include <bflibc/stringutils.h>
}

class Message;

/**
 * In charge of creating message
 *
 * This class has its own memory management
 */
class Chatroom : public BF::Object {
public:
	/// returns chat room for uuid
	static Chatroom * getChatroom(uuid_t chatroomuuid);

	/// count of all available chatrooms
	static int getChatroomsCount();

	/**
	 * caller owns memory of array and its elements
	 */
	static PayloadChatInfo ** getChatroomList(int * size, int * err);

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

protected:

	/**
	 * inits chatroom with uuid
	 */
	Chatroom();

	static void addRoomToChatrooms(Chatroom * cr);

	uuid_t _uuid;
	char _name[CHAT_ROOM_NAME_SIZE];

private:

	int getinfo(PayloadChatInfo * info);
};

#endif // CHATROOM_HPP

