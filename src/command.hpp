/**
 * author: brando
 * date: 4/6/24
 */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <bflibcpp/object.hpp>

class InputBuffer;

class Command : public BF::Object {
public:
	Command(InputBuffer & buf);
	virtual ~Command();
};

#endif // COMMAND_HPP

