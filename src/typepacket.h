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
#define DATA_BUFFER_SIZE 255

typedef enum {
	kPacketTypeMessage = 1,
} PacketType;

typedef struct {
	//Header header;
	struct {
		// struct version
		unsigned char version;
		
		/**
		 * the time this message was sent
		 */
		BFTime time;
	} header;

	union {
		struct {

			/**
			 * sender's user name
			 */
			char username[USER_NAME_SIZE];

			/**
			 * sender's user uuid
			 */
			char useruuid[kBFStringUUIDStringLength];

			/**
			 * chat room uuid
			 */
			char chatuuid[kBFStringUUIDStringLength];

			char data[DATA_BUFFER_SIZE];
		} message;
	} payload;
} Packet;

#define PACKET_ALLOC (Packet *) malloc(sizeof(Packet))
#define PACKET_FREE(...) free(__VA_ARGS__)

#endif // TYPE_PACKET_H

