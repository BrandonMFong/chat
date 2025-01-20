/**
 * author: brando
 * date: 1/9/25
 */

#include "operand.hpp"
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

const Operand OP_HELP("help", '?');
const Operand OP_CREATE("create");
const Operand OP_JOIN("join");
const Operand OP_LEAVE("leave");
const Operand OP_DRAFT("draft", 'i');
const Operand OP_QUIT("quit", 'q');

Operand::Operand(const char * vlong, char vshort) {
	strcpy(this->_long, vlong);
	this->_short = vshort;
}

Operand::Operand(const char * vlong) : Operand(vlong, 0) { }

Operand::Operand(char vshort) : Operand("", vshort) { }

Operand::~Operand() { }

bool Operand::compare(const Operand & op) {
	bool long_mask = strlen(this->_long) > 0 && strlen(op._long) > 0 && !strcmp(this->_long, op._long);
	bool short_mask = this->_short != 0 && op._short != 0 && this->_short == op._short;
	return long_mask || short_mask;
}

bool Operand::operator==(const Operand & op) {
	return this->compare(op);
}

bool Operand::operator!=(const Operand & op) {
	return !this->compare(op);
}

String Operand::description() const {
	return String("%s - %c", this->_long, this->_short);
}

