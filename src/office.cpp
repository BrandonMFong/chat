/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "chat.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "message.hpp"
#include "agent.hpp"
#include "log.hpp"
#include "packet.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

int Office::quitApplication(const User * user) {
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeNotifyQuitApp);

	PayloadUserInfo info;
	user->getuserinfo(&info);
	PacketSetPayload(&p, &info);

	return Agent::broadcast(&p);
}

