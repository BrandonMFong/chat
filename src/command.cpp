/**
 * author: brando
 * date: 4/6/24
 */

#include "command.hpp"
#include "inputbuffer.hpp"
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

void _CommandReleaseArray(char * s) {
	BFFree(s);
}

Command::Command(InputBuffer & buf) : Object() {
	this->_args.setReleaseCallback(_CommandReleaseArray);

	size_t size = 0;
	char ** arr = BFStringCreateArrayFromString(buf.cString(), &size, " ");

	this->_args.set(arr, size);

	BFFree(arr);
}

Command::~Command() {

}

String Command::op() const {
	const char * res = this->_args[0];
	if (res) return res;
	return "";
}

