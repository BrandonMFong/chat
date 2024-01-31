/**
 * author: brando
 * date: 1/24/24
 */

#ifndef TYPE_CHAT_CONFIG_H
#define TYPE_CHAT_CONFIG_H

#include <bflibcpp/queue.hpp>
#include <bflibcpp/atomic.hpp>
#include <typepacket.h>

typedef struct {
	// Each queue will be responsible for managing
	// their message memory
	//
	// Each message in the queues are dynamically allocated
	// by malloc
	BF::Atomic<BF::Queue<Packet *>> in;
	BF::Atomic<BF::Queue<Packet *>> out;

	char username[255];


} ChatConfig;

#endif // TYPE_CHAT_CONFIG_H

