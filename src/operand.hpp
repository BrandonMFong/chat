/**
 * author: brando
 * date: 1/9/25
 */

#ifndef OPERAND_HPP
#define OPERAND_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/string.hpp>

/**
 * I want the user to be able to pass '?' or ":help" in the
 * command prompt to get the help menu
 */
class Operand : public BF::Object {
public:
	Operand(std::initializer_list<const char *> list);
	virtual ~Operand();

	bool compare(const Operand & op);

	BF::String description() const;

private:
	BF::Array<char *> _acceptedArgs;

public:
	bool operator==(const Operand & op);
	bool operator!=(const Operand & op);
};

extern const Operand OP_HELP;
extern const Operand OP_CREATE;
extern const Operand OP_JOIN;
extern const Operand OP_LEAVE;
extern const Operand OP_DRAFT;
extern const Operand OP_QUIT;

#endif // OPERAND_HPP

