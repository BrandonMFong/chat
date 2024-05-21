/**
 * author: brando
 * date: 2/20/24
 */

#include "office.hpp"
#include "chat.hpp"
#include <bfnet/bfnet.hpp>
#include "user.hpp"
#include "chatroom.hpp"
#include "message.hpp"
#include "agent.hpp"
#include "log.hpp"
#include "packet.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;
using namespace BF::Net;

Atomic<Queue<SocketEnvelope *>> inbox;
BFLock inboxlock;
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

void Office::packetReceive(SocketEnvelope * envelope) {
	// push into queue
	BFRetain(envelope);
	inbox.get().push(envelope);
	BFLockRelease(&inboxlock);
}

void _OfficeInDataWorkerThread(void * in) {
	while (!BFThreadAsyncIsCanceled(_tid)) {
		if (inbox.get().empty()) { 
			BFLockWait(&inboxlock);
		} else {
			inbox.lock();

			// get first item from the queue
			SocketEnvelope * envelope = inbox.unsafeget().front();

			// send it over to the agents
			Agent::packetReceive(envelope);

			// pop off
			inbox.unsafeget().pop();

			// release memory
			BFRelease(envelope);

			inbox.unlock();
		}
	}
}

int Office::start() {
	_tid = BFThreadAsync(_OfficeInDataWorkerThread, NULL);
	BFLockCreate(&inboxlock);
	return 0;
}

int Office::stop() {
	BFLockRelease(&inboxlock);
	BFThreadAsyncCancel(_tid);
	BFThreadAsyncWait(_tid);
	BFThreadAsyncDestroy(_tid);
	BFLockDestroy(&inboxlock);
	return 0;
}

