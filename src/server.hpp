/**
 * author: brando
 * date: 1/24/24
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "socket.hpp"

class Server : public Socket {
public:
	Server();
	virtual ~Server();
	static void init(void * in);
	const char mode() const;
	const int descriptor() const;

protected:
	int _start();
	int _stop();

private:
	int _mainSocket;
	int _clientSocket;
};

#endif // SERVER_HPP

