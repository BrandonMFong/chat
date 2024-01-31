/**
 * author: brando
 * date: 1/25/24
 */

#include <interface.hpp>
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>

using namespace BF;

int InterfaceOutStreamAddMessage(ChatConfig * config, Packet * pkt) {
	if (!config || !pkt) return -2;

	Packet * p = PACKET_ALLOC;
	if (!p) return -2;

	memcpy(p, pkt, sizeof(Packet));

	config->out.lock();
	int error = config->out.get().push(p);
	config->out.unlock();
	return error;
}

void InterfaceInStreamThread(void * in) {
	ChatConfig * config = (ChatConfig *) in;

	while (1) {
		config->in.lock();
		// if queue is not empty, send the next message
		if (!config->in.get().empty()) {
			// get first message
			Packet * p = config->in.get().front();

			printf("> %s", p->payload.message.buf);
			fflush(stdout);

			// pop queue
			config->in.get().pop();

			PACKET_FREE(p);
		}
		config->in.unlock();
	}
}

int InterfaceReadInput(Packet * p) {
	if (!p) return -30;
	printf("> ");
	fgets(
		p->payload.message.buf,
		sizeof(p->payload.message.buf),
		stdin
	);

	if (strlen(p->payload.message.buf)
	&& p->payload.message.buf[strlen(p->payload.message.buf) - 1] == '\n') {
		p->payload.message.buf[strlen(p->payload.message.buf) - 1] = '\0';
	}

	return 0;
}

int InterfaceRun(ChatConfig * config) {
	int i = 0;
	int error = 0;

	// this thread will monitor incoming messages from the in q
	BFThreadAsyncID tid = BFThreadAsync(InterfaceInStreamThread, (void *) config);

	while (!error) {
		Packet p;

		error = InterfaceReadInput(&p);

		if (!error)
			error = InterfaceOutStreamAddMessage(config, &p);

		i++;
	}

	return error;
}

