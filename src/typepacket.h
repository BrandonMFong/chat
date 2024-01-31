/**
 * author: brando
 * date: 1/25/24
 */

#ifndef TYPE_PACKET_H
#define TYPE_PACKET_H

#include <chat.h>

typedef struct {
	union {
		struct {
			char username[255];
			char buf[MESSAGE_BUFFER_SIZE];
		} message;
	} payload;
} Packet;

#define PACKET_ALLOC (Packet *) malloc(sizeof(Packet))
#define PACKET_FREE(...) free(__VA_ARGS__)

#endif // TYPE_PACKET_H

