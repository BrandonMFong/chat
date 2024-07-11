/**
 * author: brando
 * date: 3/1/24
 */

#include "packet.hpp"
#include "typepacket.h"
#include <string.h>

extern "C" {
#include <bflibc/bflibc.h>
}

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

	case kPayloadTypeChatroomEnrollmentRequest:
		memcpy(
			&pkt->payload.enrollreq,
			buf,
			sizeof(pkt->payload.enrollreq)
		);
		return 0;
	default:
		return 3;
	}
}

