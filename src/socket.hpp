/**
 * author: brando
 * date: 2/1/24
 */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "typechatconfig.h"

#define SOCKET_MODE_SERVER 's'
#define SOCKET_MODE_CLIENT 'c'

class Socket {
public: 
	static Socket * create(const char mode, int * err);
	virtual ~Socket();

	virtual int start(ChatConfig * config) = 0;
	virtual int stop(ChatConfig * config) = 0;

	virtual const char mode() const = 0;
	
protected:
	Socket();
};

#endif // SOCKET_HPP

