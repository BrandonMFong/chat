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
	
	int start(ChatConfig * config);
	int stop(ChatConfig * config);

	const char mode() const;
};

#endif // CLIENT_HPP

