/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "chat.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"
#include "message.hpp"
#include "agent.hpp"
#include "log.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

int Office::PacketSend(const Packet * p) {
	// TODO: this is agent's job
	//return Socket::shared()->sendData(p, sizeof(Packet));
	return 0;
}


