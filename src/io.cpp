/**
 * author: brando
 * date: 1/26/24
 */

#include "io.hpp"
#include "chat.h"
#include "log.h"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

void IOIn(void * in) {
	IOTools * tools = (IOTools *) in;
	
	while (1) {
		char buf[MESSAGE_BUFFER_SIZE];
        if (recv(tools->cd, buf, sizeof(buf), 0) == -1) {
			ELog("%d\n", errno);
			break;
		}

		Packet * p = PACKET_ALLOC;
		memcpy(p->payload.message.buf, buf, MESSAGE_BUFFER_SIZE);

		tools->config->in.lock();
		tools->config->in.get().push(p);
		tools->config->in.unlock();
	}
}

void IOOut(void * in) {
	IOTools * tools = (IOTools *) in;

	while (1) {
		tools->config->out.lock();
		// if queue is not empty, send the next message
		if (!tools->config->out.get().empty()) {
			// get first message
			Packet * p = tools->config->out.get().front();

			// pop queue
			tools->config->out.get().pop();

			// send buf from message
			send(tools->cd, p->payload.message.buf, sizeof(p->payload.message.buf), 0);

			PACKET_FREE(p);
		}
		tools->config->out.unlock();
	}
}

