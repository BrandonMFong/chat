/**
 * author: brando
 * date: 1/25/24
 */

#ifndef TYPE_PACKET_H
#define TYPE_PACKET_H

#include "typemessage.h"

typedef struct {
	union {
		Message message;
	} payload;
} Packet;

#define PACKET_ALLOC (Packet *) malloc(sizeof(Packet))
#define PACKET_FREE(...) free(__VA_ARGS__)

#endif // TYPE_PACKET_H

