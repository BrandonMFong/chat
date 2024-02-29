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

Chatroom * ChatDirectoryGetChatroom(uuid_t chatroomuuid) {
	Chatroom * room = NULL;
	chatrooms.lock();
	List<Chatroom *>::Node * n = chatrooms.unsafeget().first();
	for (; n != NULL; n = n->next()) {
		Chatroom * troom = n->object();
		uuid_t uuid;
		troom->getuuid(uuid);
		if (!uuid_compare(uuid, chatroomuuid)) {
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

