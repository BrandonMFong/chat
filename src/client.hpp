/**
 * author: brando
 * date: 1/24/24
 */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "socket.hpp"

extern "C" {
#include <bflibc/typethreadid.h>
}

class Client : public Socket {
public:
	Client();
	virtual ~Client();
	const int descriptor() const;
	const char mode() const;
protected:
	static void init(void * in);
	int _start();
	int _stop();
private:
};

#endif // CLIENT_HPP

