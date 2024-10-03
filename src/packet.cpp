/**
 * author: brando
 * date: 3/1/24
 */

#include "packet.hpp"
#include "inputbuffer.hpp"
#include "user.hpp"
#include "typepacket.h"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

extern "C" {
#include <bflibc/bflibc.h>
}

using namespace BF;

int PacketSetHeader(Packet * pkt, PayloadType type) {
	if (!pkt)
		return 1;

	pkt->header.version = 0;
	pkt->header.time = BFTimeGetCurrentTime();
	pkt->header.type = type;
	pkt->header.frameno = 0xff;

	return 0;
}

int PacketSetPayload(Packet * pkt, const void * buf) {
	if (!pkt)
		return 2;

	switch (pkt->header.type) {
	case kPayloadTypeChatroomEnrollment:
		memcpy(
			&pkt->payload.enrollment,
			buf,
			sizeof(pkt->payload.enrollment)
		);
		return 0;

	case kPayloadTypeNotifyQuitApp:
		memcpy(
			&pkt->payload.userinfo,
			buf,
			sizeof(pkt->payload.userinfo)
		);
		return 0;

	case kPayloadTypeChatInfo:
		memcpy(
			&pkt->payload.chatinfo,
			buf,
			sizeof(pkt->payload.chatinfo)
		);
		return 0;

	case kPayloadTypeChatroomEnrollmentForm:
		memcpy(
			&pkt->payload.enrollform,
			buf,
			sizeof(pkt->payload.enrollform)
		);
		return 0;
	default:
		return 3;
	}
}

int PacketPayloadSetPayloadMessage(
	PayloadMessage * payload,
	PayloadMessageType type,
	uuid_t chatuuid,
	const char * username,
	uuid_t useruuid,
	const InputBuffer & buf
) {
	// load encrypted message data
	Data data = buf;
	memcpy(payload->data, data.buffer(), data.size());

	// username
	strncpy(
		payload->username,
		username,
		sizeof(payload->username)
	);

	// user uuid
	//user->getuuid(payload->useruuid);	
	uuid_copy(payload->useruuid, useruuid);

	// chatroom uuid
	uuid_copy(payload->chatuuid, chatuuid);

	// message type
	payload->type = type;


	return 0;
}

