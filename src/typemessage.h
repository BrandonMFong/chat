/**
 * author: brando
 * date: 2/5/24
 */

#ifndef TYPE_MESSAGE_H
#define TYPE_MESSAGE_H

extern "C" {
#include <bflibc/bftime.h>
#include <bflibc/stringutils.h>
}

#define MESSAGE_BUFFER_SIZE 255
#define USER_NAME_SIZE 255

typedef struct {
	// message struct version
	unsigned char version;

	/**
	 * this will hold the raw message content
	 *
	 * this buffer should NOT end in '\n'
	 */
	char buf[MESSAGE_BUFFER_SIZE];

	/**
	 * sender's user name
	 */
	char username[USER_NAME_SIZE];

	/**
	 * the time this message was sent
	 */
	BFTime time;

	/**
	 * chat room uuid
	 */
	char chatuuid[kBFStringUUIDStringLength];
} Message;

#define MESSAGE_ALLOC (Message *) malloc(sizeof(Message))
#define MESSAGE_FREE(...) free(__VA_ARGS__)

#endif // TYPE_MESSAGE_H

