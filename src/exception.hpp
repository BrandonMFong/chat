/**
 * author: brando
 * date: 7/11/24
 */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <bflibcpp/string.hpp>

class Exception : public std::exception {
public:
	Exception(BF::String msg);
	virtual ~Exception();

	virtual const char * msg() const throw();

private:
	BF::String _msg;
};

#endif // EXCEPTION_HPP

