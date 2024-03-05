/**
 * author: brando
 * date: 2/14/24
 */

#include "inputbuffer.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "user.hpp"
#include "log.hpp"
#include <ncurses.h>

using namespace BF;

InputBuffer::InputBuffer() : String("") {
	this->_isready = false;
	this->_cursorpos = 0;
}

InputBuffer::~InputBuffer() {

}

int InputBuffer::addChar(int ch) {
	switch (ch) {
	case '\n':
		this->_isready = true;
		break;
	case KEY_BACKSPACE:
		if (this->_cursorpos > 0) {
			this->String::remCharAtIndex(this->_cursorpos - 1);
			this->_cursorpos--;
		}
		break;
	case KEY_LEFT:
		this->_cursorpos--;
		break;
	case KEY_RIGHT:
		this->_cursorpos++;
		break;
	case ERR:
		break;
	default:
		// If a key is pressed (excluding Enter), add it to the userInput string
		this->String::addCharAtIndex(ch, this->_cursorpos);
		this->_cursorpos++;
		break;
	}

	return 0;
}

int InputBuffer::reset() {
	this->_cursorpos = 0;
	this->_isready = false;
	return this->String::clear();
}

size_t InputBuffer::cursorPosition() {
	return this->_cursorpos;
}

bool InputBuffer::isready() {
	return this->_isready;
}

