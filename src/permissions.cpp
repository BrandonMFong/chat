/**
 * author: brando
 * date: 3/20/24
 */

#include "permissions.hpp"
#include "chat.hpp"
#include "socket.hpp"

bool Permissions::CanCreateChatroom() {
	return Chat::SocketGetMode() == SOCKET_MODE_SERVER;
}

