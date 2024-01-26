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

	q->lock();
	int error = q->get().push(m);
	q->unlock();
	return error;
}

int MessengerRun(ChatConfig * config) {
	int i = 0;
	int error = 0;
	while (!error) {
		Message msg;

		printf("> ");
		fgets(msg.buf, sizeof(msg.buf), stdin);

		if (strlen(msg.buf) && msg.buf[strlen(msg.buf) - 1] == '\n') {
			msg.buf[strlen(msg.buf) - 1] = '\0';
		}

		error = MessengerAddMessageToQueue(&config->out, &msg);

		i++;
	}

	return error;
}

