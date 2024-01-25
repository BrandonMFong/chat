/**
 * author: brando
 * date: 1/24/24
 */

#ifndef TYPE_CHAT_CONFIG_H
#define TYPE_CHAT_CONFIG_H

#include <bflibcpp/queue.hpp>
#include <bflibcpp/atomic.hpp>

typedef struct {
	BF::Atomic<BF::Queue<char *>> msgIn;
	BF::Atomic<BF::Queue<char *>> msgOut;
	int sd; // socket descriptor
} ChatConfig;

#endif // TYPE_CHAT_CONFIG_H

