/**
 * author: brando
 * date: 1/25/24
 */

#ifndef TYPE_MESSAGE_H
#define TYPE_MESSAGE_H

#include <chat.h>

#define MESSAGE_ALLOC(...) malloc(__VA_ARGS__)
#define MESSAGE_FREE(...) free(__VA_ARGS__)

typedef struct {
	char buf[MESSAGE_BUFFER_SIZE];
} Message;

#endif // TYPE_MESSAGE_H

