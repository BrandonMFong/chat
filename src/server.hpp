/**
 * author: brando
 * date: 1/24/24
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "socket.hpp"

extern "C" {
#include <bflibc/typethreadid.h>
}

class Server : public Socket {
public:
	Server();
	virtual ~Server();
	const char mode() const;
	const int descriptor() const;

protected:
	static void init(void * in);
	int _start();
	int _stop();

private:
	int _mainSocket;
	int _clientSocket;
};

#endif // SERVER_HPP

