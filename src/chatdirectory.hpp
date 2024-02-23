/**
 * author: brando
 * date: 2/20/24
 */

#ifndef CHAT_DIRECTORY_HPP
#define CHAT_DIRECTORY_HPP

class Chatroom;

Chatroom * ChatDirectoryGetChatroom(const char * chatroomuuid);
void ChatDirectoryAddChatroom(Chatroom * room);

int ChatDirectoryCount();

#endif // CHAT_DIRECTORY_HPP

