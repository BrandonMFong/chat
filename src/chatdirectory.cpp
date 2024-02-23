/**
 * author: brando
 * date: 2/20/24
 */

#include "chatdirectory.hpp"
#include "chatroom.hpp"
#include "log.hpp"
#include <string.h>

using namespace BF;

Atomic<List<Chatroom *>> chatrooms;

int ChatDirectoryCount() {
	return chatrooms.get().count();
}

Chatroom * ChatDirectoryGetChatroom(const char * chatroomuuid) {
	Chatroom * room = NULL;
	chatrooms.lock();
	List<Chatroom *>::Node * n = chatrooms.unsafeget().first();
	for (; n != NULL; n = n->next()) {
		Chatroom * troom = n->object();
		if (!BFStringCompareUUID(troom->uuid(), chatroomuuid)) {
			room = troom;
			break;
		}
	}
	chatrooms.unlock();

	return room;
}

void ChatDirectoryAddChatroom(Chatroom * room) {
	chatrooms.get().add(room);
}

