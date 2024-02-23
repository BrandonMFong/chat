/**
 * author: brando
 * date: 1/25/24
 */

#ifndef TYPE_PACKET_H
#define TYPE_PACKET_H

extern "C" {
#include <bflibc/bftime.h>
#include <bflibc/stringutils.h>
}

#define USER_NAME_SIZE 255

typedef struct {
	// struct version
	unsigned char version;

	/**
	 * sender's user name
	 */
	char username[USER_NAME_SIZE];

	/**
	 * sender's user uuid
	 */
	char useruuid[kBFStringUUIDStringLength];

	/**
	 * the time this message was sent
	 */
	BFTime time;

	/**
	 * chat room uuid
	 */
	char chatuuid[kBFStringUUIDStringLength];
} Header;

#define HEADER_ALLOC (Header *) malloc(sizeof(Header))
#define HEADER_FREE(...) free(__VA_ARGS__)

#define DATA_BUFFER_SIZE 255

typedef struct {
	Header header;
	char data[DATA_BUFFER_SIZE];
} Packet;

#define PACKET_ALLOC (Packet *) malloc(sizeof(Packet))
#define PACKET_FREE(...) free(__VA_ARGS__)

#endif // TYPE_PACKET_H

