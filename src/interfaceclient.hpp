/**
 * author: brando
 * date: 3/3/24
 */

#ifndef INTERFACE_CLIENT_HPP
#define INTERFACE_CLIENT_HPP

#include "interface.hpp"

class InterfaceClient : public Interface {
	friend class Interface;
public:
	virtual ~InterfaceClient();

private:
	InterfaceClient();
};

#endif // INTERFACE_CLIENT_HPP

