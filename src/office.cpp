/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "chat.hpp"
#include "socket.hpp"
#include "user.hpp"
#include "chatroom.hpp"
#include "message.hpp"
#include "agent.hpp"
#include "log.hpp"
#include "packet.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<Queue<Office::InData *>> _indataq;
BFThreadAsyncID _tid = NULL;

int Office::quitApplication(const User * user) {
	Packet p;
	memset(&p, 0, sizeof(p));
	PacketSetHeader(&p, kPayloadTypeNotifyQuitApp);

	PayloadUserInfo info;
	user->getuserinfo(&info);
	PacketSetPayload(&p, &info);

	return Agent::broadcast(&p);
}

void Office::packetReceive(SocketConnection * sc, const void * buf, size_t size) {
	// make data
	InData * data = (InData *) malloc(sizeof(InData));
	data->sc = sc;
	data->size = size;
	data->buf = malloc(size);
	memcpy(data->buf, buf, size);

	// push into queue
	_indataq.get().push(data);
}

void _OfficeInDataWorkerThread(void * in) {
	while (!BFThreadAsyncIsCanceled(_tid)) {
		if (!_indataq.get().empty()) {
			_indataq.lock();

			// get first item from the queue
			Office::InData * data = _indataq.unsafeget().front();

			// send it over to the agents
			Agent::packetReceive(data->sc, data->buf, data->size);

			// pop off
			_indataq.unsafeget().pop();

			// release memory
			BFFree(data->buf);
			BFFree(data);

			_indataq.unlock();
		}
	}
}

int Office::start() {
	_tid = BFThreadAsync(_OfficeInDataWorkerThread, NULL);
	return 0;
}

int Office::stop() {
	BFThreadAsyncCancel(_tid);
	BFThreadAsyncWait(_tid);
	BFThreadAsyncDestroy(_tid);
	return 0;
}

