/**
 * author: brando
 * date: 1/24/24
 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "socket.hpp"

class Client : public Socket {
public:
	Client();
	virtual ~Client();
	static void init(void * in);
	int start();
	int stop();
	const int descriptor() const;
	const char mode() const;
private:
	int _mainSocket;
};

#endif // CLIENT_HPP

