/**
 * author: brando
 * date: 2/20/24
 */

#ifndef CHAT_DIRECTORY_HPP
#define CHAT_DIRECTORY_HPP

#include <uuid/uuid.h>

class Chatroom;

Chatroom * ChatDirectoryGetChatroom(uuid_t chatroomuuid);
void ChatDirectoryAddChatroom(Chatroom * room);

int ChatDirectoryCount();

#endif // CHAT_DIRECTORY_HPP

