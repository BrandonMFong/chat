/**
 * author: brando
 * date: 1/25/24
 */

#include <messenger.hpp>
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>

using namespace BF;

int MessengerOutStreamAddMessage(ChatConfig * config, Packet * msg) {
	if (!config || !msg) return -2;

	Packet * m = PACKET_ALLOC;
	if (!m) return -2;

	memcpy(m, msg, sizeof(Packet));

	config->out.lock();
	int error = config->out.get().push(m);
	config->out.unlock();
	return error;
}

void MessengerInStreamThread(void * in) {
	ChatConfig * config = (ChatConfig *) in;

	while (1) {
		config->in.lock();
		// if queue is not empty, send the next message
		if (!config->in.get().empty()) {
			// get first message
			Packet * p = config->in.get().front();

			printf("> %s", p->buf);
			fflush(stdout);

			// pop queue
			config->in.get().pop();

			PACKET_FREE(p);
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
		Packet p;

		printf("> ");
		fgets(p.buf, sizeof(p.buf), stdin);

		if (strlen(p.buf) && p.buf[strlen(p.buf) - 1] == '\n') {
			p.buf[strlen(p.buf) - 1] = '\0';
		}

		error = MessengerOutStreamAddMessage(config, &p);

		i++;
	}

	return error;
}

