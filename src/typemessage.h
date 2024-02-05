/**
 * author: brando
 * date: 2/5/24
 */

#ifndef TYPE_MESSAGE_H
#define TYPE_MESSAGE_H

#include <bflibc/bftime.h>

#define MESSAGE_BUFFER_SIZE 255

typedef struct {
	char buf[MESSAGE_BUFFER_SIZE];
	BFTime time;
} Message;

#endif // TYPE_MESSAGE_H

