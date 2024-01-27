/**
 * author: brando
 * date: 1/26/24
 */

#include "io.hpp"
#include "chat.h"
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>


void IOIn(void * in) {
	IOTools * tools = (IOTools *) in;
	
	int i = 0;
	while (i < 10) {
		char buf[MESSAGE_BUFFER_SIZE];
        recv(tools->cd, buf, sizeof(buf), 0);
		i++;
	}
}

void IOOut(void * in) {
	IOTools * tools = (IOTools *) in;

	while (1) {
		tools->config->out.lock();
		// if queue is not empty, send the next message
		if (!tools->config->out.get().empty()) {
			// get first message
			Message * msg = tools->config->out.get().front();

			// pop queue
			tools->config->out.get().pop();

			// send buf from message
			send(tools->cd, msg->buf, sizeof(msg->buf), 0);

			MESSAGE_FREE(msg);
		}
		tools->config->out.unlock();
	}
}


