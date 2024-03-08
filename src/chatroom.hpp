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
class User;

/**
 * In charge of creating message
 *
 * This class has its own memory management
 */
class Chatroom : public BF::Object {
public:
	/** returns chat room for uuid
	 *
	 * caller does not own but returned object can be retained
	 */
	static Chatroom * getChatroom(const uuid_t chatroomuuid);

	/// count of all available chatrooms
	static int getChatroomsCount();

	/**
	 * caller owns memory of array and its elements
	 */
	static PayloadChatInfo ** getChatroomList(int * size, int * err);

	virtual ~Chatroom();

	/**
	 * constructs a packet to send to all receivers
	 *
	 * this function also calls `addMessage`
	 */
	int sendBuffer(const InputBuffer * buf);

	/**
	 * constructs message from packet and adds to our list of messages
	 */
	virtual int receiveMessagePacket(const Packet * pkt);

	/**
	 * returns uuid
	 */
	void getuuid(uuid_t uuid);

	/**
	 * enrolls user to chatroom and communicates with everyone in
	 * the chatroom, currently, that `user` joined
	 */
	int enroll(const User * user);

	BF::Atomic<BF::List<Message *>> conversation;

protected:

	/**
	 * adds message and flags an update
	 *
	 * msg : memory must be created before calling
	 */
	int addMessage(Message * msg);

	/**
	 * inits chatroom with uuid
	 * uuid : gets copied
	 */
	Chatroom(const uuid_t uuid);

	/**
	 * creates chatroom and sets random uuid
	 */
	Chatroom();

	static void addRoomToChatrooms(Chatroom * cr);

	/**
	 * the two subclasses have different amount of
	 * agents
	 *
	 * chatroom on the client side will always have 
	 * one agent, whereas the server will have many
	 */
	virtual int sendPacket(const Packet * pkt) = 0;

	uuid_t _uuid;
	char _name[CHAT_ROOM_NAME_SIZE];

private:
	/**
	 * list of users in chatroom
	 */
	BF::Atomic<BF::List<User *>> _users;

	int getinfo(PayloadChatInfo * info);
};

#endif // CHATROOM_HPP

