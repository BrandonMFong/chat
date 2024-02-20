/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"
#include <string.h>

int Office::PacketReceive(const Packet * pkt) {
	return 0;
}

int Office::BufferSend(const InputBuffer * buf) {
	Packet p;

	// clear packet
	memset(&p, 0, sizeof(p));
	// load buffer 
	strncpy(p.payload.message.buf, buf->cString(), sizeof(p.payload.message.buf));

	// username
	strncpy(p.payload.message.username, User::current()->username(), sizeof(p.payload.message.username));

	// time
	p.payload.message.time = BFTimeGetCurrentTime();

	Chatroom * chatroom = ChatDirectory::shared()->getChatroom();
	if (!chatroom) return 30;

	// give chatroom this message to add to 
	// its list
	chatroom->addMessage(&p.payload.message);

	// send to socket
	return Socket::shared()->sendPacket(&p);
}

