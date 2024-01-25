/**
 * author: brando
 * date: 1/25/24
 */

#include <messenger.hpp>
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>

using namespace BF;

int MessengerAddMessageToQueue(Atomic<Queue<Message *>> * q, const Message * msg) {
	if (!q || !msg) return -2;

	Message * m = (Message *) MESSAGE_ALLOC(sizeof(Message));
	if (!m) return -2;

	memcpy(m, msg, sizeof(Message));
	return q->get().push(m);
}

int MessengerRun(ChatConfig * config) {
	int i = 0;
	int error = 0;
	while (!error) {
		Message msg;
		snprintf(msg.buf, sizeof(msg.buf), "server %d", i);
		error = MessengerAddMessageToQueue(&config->out, &msg);
		sleep(1);
		i++;
	}
	return error;
}

