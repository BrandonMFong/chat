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

const Operand OP_HELP({"help", "?"});
const Operand OP_CREATE({"create"});
const Operand OP_JOIN({"join"});
const Operand OP_LEAVE({"leave"});
const Operand OP_DRAFT({"draft", "i"});
const Operand OP_QUIT({"quit", "q"});

void _OperandAcceptArgsRelease(char * a) {
	BFFree(a);
}

Operand::Operand(std::initializer_list<const char *> list) : Object() {
	this->_acceptedArgs.setReleaseCallback(_OperandAcceptArgsRelease);
	for (const char * arg : list) {
		char * buf = BFStringCopyString(arg);
		this->_acceptedArgs.add(buf);
	}
}

Operand::~Operand() { }

bool Operand::compare(const Operand & op) {
	return false;
}

bool Operand::operator==(const Operand & op) {
	return this->compare(op);
}

bool Operand::operator!=(const Operand & op) {
	return !this->compare(op);
}

String Operand::description() const {
	return String("unknown");
}

