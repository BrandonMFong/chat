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

	memset(&pkt, 0, sizeof(Packet));
	pkt->header.version = 0;
	pkt->header.time = BFTimeGetCurrentTime();
	pkt->header.type = type;

	return 0;
}

int PacketSetPayload(Packet * pkt, const void * buf) {
	if (!pkt)
		return 2;

	switch (pkt->header.type) {
	case kPayloadTypeMessage:
		break;
	case kPayloadTypeRequestUserInfo:
		break;
	case kPayloadTypeUserInfo:
		break;
	case kPayloadTypeRequestAvailableChatrooms:
		break;
	case kPayloadTypeChatroomInfo:
		memcpy(
			&pkt->payload.chatroominfobrief,
			buf,
			sizeof(pkt->payload.chatroominfobrief)
		);
		break;
	default:
		break;
	}

	return 0;
}

