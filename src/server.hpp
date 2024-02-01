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
	int start();
	int stop();
	const char mode() const;
};

#endif // SERVER_HPP

