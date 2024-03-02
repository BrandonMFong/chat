/**
 * author: brando
 * date: 3/2/24
 */

#ifndef CHATROOM_CLIENT_HPP
#define CHATROOM_CLIENT_HPP

#include "chatroom.hpp"

class ChatroomClient : public Chatroom {
public:
	static int recordChatroom(const PayloadChatroomInfoBrief * info);
	virtual ~ChatroomClient();
private:
	ChatroomClient();
};

#endif // CHATROOM_CLIENT_HPP

