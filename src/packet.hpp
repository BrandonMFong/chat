/**
 * author: brando
 * date: 3/1/24
 */

#ifndef PACKET_HPP
#define PACKET_HPP

#include "typepacket.h"

class User;
class InputBuffer;

/**
 * Sets some packet values like the header
 *
 * automatically sets time to current
 */
int PacketSetHeader(Packet * pkt, PayloadType type);

/**
 * sets payload value to buf
 *
 * size of buf is determined automatically
 *
 * buf is copied
 */
int PacketSetPayload(Packet * pkt, const void * buf);

/**
 * Loads PayloadMessage with the parameters
 */
int PacketPayloadSetPayloadMessage(
	PayloadMessage * payload,
	PayloadMessageType type,
	uuid_t chatuuid,
	const char * username,
	uuid_t useruuid,
	const InputBuffer & buf
);

#endif // PACKET_HPP

