/**
 * author: brando
 * date: 1/9/25
 */

#include "operand.hpp"
#include <bflibcpp/bflibcpp.hpp>

extern "C" {
#include <bflibc/bflibc.h>
}

using namespace BF;

const Operand OP_HELP({OPERAND_STRING_LONG_HELP, "?"});
const Operand OP_CREATE({OPERAND_STRING_LONG_CREATE});
const Operand OP_JOIN({OPERAND_STRING_LONG_JOIN});
const Operand OP_LEAVE({OPERAND_STRING_LONG_LEAVE});
const Operand OP_DRAFT({OPERAND_STRING_LONG_DRAFT, "i"});
const Operand OP_QUIT({OPERAND_STRING_LONG_QUIT, "q"});

void _OperandAcceptArgsRelease(char * a) {
	BFFree(a);
}

int _OperandAcceptArgsCompare(char * a, char * b) {
	return strcmp(a, b);
}

Operand::Operand(std::initializer_list<String> list) : Object() {
	this->_acceptedArgs.setReleaseCallback(_OperandAcceptArgsRelease);
	for (const String & arg : list) {
		this->_acceptedArgs.add(arg.cStringCopy());
	}
	this->_acceptedArgs.setComparator(_OperandAcceptArgsCompare);
}

Operand::~Operand() { }

bool Operand::compare(const Operand & op) {
	// FIXME:
	// this is a poor implementation. time efficiency will decrease
	// as more accepted arguments are implmented. I suggested to use
	// algorithms as presented here:https://stackoverflow.com/a/245521/12135693
	//
	// current implementation for BF::Array doesn't support the scope of those
	// algorithms
	for (int i = 0; i < this->_acceptedArgs.count(); i++) {
		if (op._acceptedArgs.contains(this->_acceptedArgs[i])) 
			return true;
	}
	return false;
}

bool Operand::operator==(const Operand & op) {
	return this->compare(op);
}

bool Operand::operator!=(const Operand & op) {
	return !this->compare(op);
}

String Operand::description() const {
	return String("%s", this->_acceptedArgs[0]);
}

