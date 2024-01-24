/**
 * author: brando
 * date: 1/24/24
 */

#ifndef TYPE_CHAT_CONFIG_H
#define TYPE_CHAT_CONFIG_H

#include <bflibcpp/queue.hpp>
#include <bflibcpp/atomic.hpp>

typedef struct {
	Atomic<Queue<char *> msgIn;
	Atomic<Queue<char *> msgOut;

} ChatConfig;

#endif // TYPE_CHAT_CONFIG_H

