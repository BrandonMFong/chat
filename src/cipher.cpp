/**
 * author: brando
 * date: 5/24/24
 */

#include <bflibcpp/bflibcpp.hpp>
#include "cipher.hpp"
#include <string.h>
#include <stdlib.h>

Cipher::Cipher(const void * data, size_t size) {
	this->_dataPlainSize = size;
	
	this->_dataPlain = malloc(sizeof(char) * size);
	if (this->_dataPlain == NULL)
		return;

	memcpy(this->_dataPlain, data, size);
}

Cipher::~Cipher() { 
	BFFree(this->_dataPlain);
}

bool Cipher::isEncrypted() {
	return false;
}

const void * Cipher::data() {
	return this->_dataPlain;
}

size_t Cipher::size() {
	return this->_dataPlainSize;
}

