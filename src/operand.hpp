/**
 * author: brando
 * date: 1/9/25
 */

#ifndef OPERAND_HPP
#define OPERAND_HPP

#include <bflibcpp/object.hpp>

/**
 * I want the user to be able to pass '?' or ":help" in the
 * command prompt to get the help menu
 */
class Operand : public BF::Object {
public:
	Operand(const char * vlong, char vshort);
	virtual ~Operand();

private:
	char _long[32];
	char _short;
};

#endif // OPERAND_HPP

