/**
 * author: brando
 * date: 1/9/25
 */

#include "operand.hpp"
#include <bflibcpp/bflibcpp.hpp>

Operand::Operand(const char * vlong, char vshort) {
	strcpy(this->_long, vlong);
	this->_short = vshort;
}

Operand::~Operand() {

}

