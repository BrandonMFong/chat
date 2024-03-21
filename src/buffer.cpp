/**
 * author: brando
 * date: 3/13/24
 */

#include "buffer.hpp"
#include <bflibcpp/bflibcpp.hpp>

SocketBuffer::SocketBuffer(size_t size) {
	this->_data = malloc(size);
	this->_size = size;
}

SocketBuffer::~SocketBuffer() {
	BFFree(this->_data);
}

const void * SocketBuffer::data() const {
	return this->_data;
}

size_t SocketBuffer::size() const {
	return this->_size;
}

