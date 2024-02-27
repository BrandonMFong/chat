/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"
#include "message.hpp"
#include "agent.hpp"
#include "agentlist.hpp"
#include "log.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

void _OfficeReceivedPayloadTypeMessage(const Packet * p) {
	// chatroom will own this memory
	Message * m = new Message(p);
	if (!m) {
		LOG_DEBUG("couldn't create message object");
		return;
	}

	Chatroom * chatroom = ChatDirectoryGetChatroom(m->chatuuid());
	if (!chatroom) {
		LOG_DEBUG("chatroom not available");
		return;
	}

	int err = chatroom->addMessage(m);
	if (err) {
		LOG_DEBUG("error adding message to chatroom: %d", err);
		return;
	}
}

void Office::PacketReceive(const void * buf, size_t size) {
	const Packet * p = (const Packet *) buf;
	if (!p) {
		LOG_DEBUG("data is null");
		return;
	}

	switch (p->header.type) {
	case kPayloadTypeMessage:
		_OfficeReceivedPayloadTypeMessage(p);
		break;
	}
}

int Office::PacketSend(const Packet * p) {
	return Socket::shared()->sendData(p, sizeof(Packet));
}

int Office::NewConnection(const SocketConnection * sc) {
	AgentList::AddAgent(new Agent(sc));
	return 0;
}

