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

	Message * m = (Message *) MESSAGE_ALLOC;
	if (!m) return -2;

	memcpy(m, msg, sizeof(Message));

	q->lock();
	int error = q->get().push(m);
	q->unlock();
	return error;
}

void MessengerInStreamThread(void * in) {
	ChatConfig * config = (ChatConfig *) in;

	while (1) {
		config->in.lock();
		// if queue is not empty, send the next message
		if (!config->in.get().empty()) {
			// get first message
			Message * msg = config->in.get().front();

			printf("> %s", msg->buf);
			fflush(stdout);

			// pop queue
			config->in.get().pop();

			MESSAGE_FREE(msg);
		}
		config->in.unlock();
	}
}

int MessengerRun(ChatConfig * config) {
	int i = 0;
	int error = 0;

	// this thread will monitor incoming messages from the in q
	BFThreadAsyncID tid = BFThreadAsync(MessengerInStreamThread, (void *) config);

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

	BFThreadAsyncIDDestroy(tid);

	return error;
}

