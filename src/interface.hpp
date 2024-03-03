/**
 * author: brando
 * date: 1/25/24
 */

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "socket.hpp"
#include <typepacket.h>
#include <bflibcpp/object.hpp>

class User;

class Interface : public BF::Object {
public:
	static Interface * create(char mode);

	/**
	 * returns current interface
	 */
	static Interface * current();

	virtual ~Interface();
	int run();

	/**
	 * current user on this machine
	 */
	User * getuser();

private:
	Interface();
};

#endif // INTERFACE_HPP

