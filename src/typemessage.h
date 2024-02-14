/**
 * author: brando
 * date: 2/5/24
 */

#ifndef TYPE_MESSAGE_H
#define TYPE_MESSAGE_H

#include <bflibc/bftime.h>

#define MESSAGE_BUFFER_SIZE 255
#define USER_NAME_SIZE 255

typedef struct {
	/**
	 * this will hold the raw message content
	 *
	 * this buffer should NOT end in '\n'
	 */
	char buf[MESSAGE_BUFFER_SIZE];
	char username[USER_NAME_SIZE];
	BFTime time;
} Message;

#define MESSAGE_ALLOC (Message *) malloc(sizeof(Message))
#define MESSAGE_FREE(...) free(__VA_ARGS__)

#endif // TYPE_MESSAGE_H

