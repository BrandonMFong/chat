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
class Agent;
class Cipher;

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
	 * empties chatroom list
	 */
	static void clearChatroomList();

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
	int sendBuffer(const InputBuffer & buf);

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
	 *
	 * this sends out a broadcast to all current agents
	 *
	 * retains user
	 */
	int enroll(User * user);

	/**
	 * opposite of `enroll`
	 * 
	 * removes user from chatroom and notifies all users regarding this
	 */
	int resign(User * user);

	/**
	 * Adds agent and user to our list
	 *
	 * +1 to agent's and user's retain count
	 */
	int addAgent(Agent * a);
	int addUser(User * u);

	/**
	 * Removes agent and user from our list
	 *
	 * release agent and user from chatroom list
	 */
	int removeAgent(Agent * a);
	int removeUser(User * u);

	/**
	 * chat room name
	 */
	const char * name();

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

	/**
	 * assuming `cr` is newly created, this function will
	 * add it to the `chatrooms` list
	 *
	 * this is defined to give access to sub classes
	 */
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

	BF::Atomic<BF::List<Agent *>> _agents;

	/**
	 * helps encrypting messages
	 */
	Cipher * _cipher;

private:

	/**
	 * sends out a broadcast that user is in chatroom
	 */
	int notifyAllServerUsersOfEnrollment(User * user);

	int notifyAllServerUsersOfResignation(User * user);

	virtual int requestEnrollment(User * user) = 0;
	int finalizeEnrollment();

	/**
	 * sends out a message to all users that this
	 * user has joined the chat
	 */
	int notifyAllChatroomUsersOfEnrollment(User * user);

	int notifyAllChatroomUsersOfResignation(User * user);

	/**
	 * adds or removes agent from chatroom
	 *
	 * action : 'a' for add, 'r' for remove
	 */
	int agentAddRemove(const char action, Agent * a);
	int userAddRemove(const char action, User * user);

	/**
	 * sends buf with a type
	 */
	int sendBuffer(PayloadMessageType type, User * user, const InputBuffer & buf);

	/**
	 * list of users in chatroom
	 */
	BF::Atomic<BF::List<User *>> _users;

	int getinfo(PayloadChatInfo * info);
};

#endif // CHATROOM_HPP

