/**
 * author: brando
 * date: 3/3/24
 */

#ifndef INTERFACE_SERVER_HPP
#define INTERFACE_SERVER_HPP

#include "interface.hpp"

class InterfaceServer : public Interface {
	friend class Interface;
public:
	virtual ~InterfaceServer();

private:
	InterfaceServer();
};

#endif // INTERFACE_SERVER_HPP

