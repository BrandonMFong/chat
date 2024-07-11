/**
 * author: brando
 * date: 7/11/24
 */

#include "exception.hpp"

Exception::Exception(BF::String msg) : _msg(msg) { }
Exception::~Exception() { }

const char * Exception::msg() const throw() {
	return this->_msg.cString();
}

