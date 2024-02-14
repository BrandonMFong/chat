/**
 * author: brando
 * date: 2/14/24
 */

#include "inputbuffer.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "user.hpp"
#include <ncurses.h>

using namespace BF;

int InputBuffer::unload(Packet * pkt) {
	if (!pkt) return 50;

	strncpy(pkt->payload.message.buf, this->cString(), sizeof(pkt->payload.message.buf));
	strncpy(pkt->payload.message.username, User::current()->username(), sizeof(pkt->payload.message.username));
	Time * t = Time::createCurrent();
	pkt->payload.message.time = t->epoch();
	Delete(t);

	this->_isready = false;

	return 0;
}

int InputBuffer::addChar(char ch) {
	if (ch == '\n') {
		this->_isready = true;
	} else if (ch != ERR) {
		// If a key is pressed (excluding Enter), add it to the userInput string
		this->String::addChar(ch);
	}

	return 0;
}

bool InputBuffer::isready() {
	return this->_isready;
}

