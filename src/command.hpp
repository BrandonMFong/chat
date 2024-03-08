/**
 * author: brando
 * date: 4/6/24
 */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/array.hpp>
#include <bflibcpp/string.hpp>

class InputBuffer;

class Command : public BF::Object {
public:
	Command(InputBuffer & buf);
	virtual ~Command();

	/**
	 * main code
	 *
	 * this is the first word in the buf
	 */
	BF::String op() const;

	BF::String argumentAtIndex(int i) const;

	BF::String operator[](int index) const;

	int count() const;

private:
	BF::Array<char *> _args;
};

#endif // COMMAND_HPP

