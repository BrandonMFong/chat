/**
 * author: brando
 * date: 1/9/25
 */

#ifndef OPERAND_HPP
#define OPERAND_HPP

#include <bflibcpp/object.hpp>

class Operand : public BF::Object {
public:
	Operand(const char * vlong, char vshort);
	virtual ~Operand();

private:
	char _long[32];
	char _short;
};

#endif // OPERAND_HPP

