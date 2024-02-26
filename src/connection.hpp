/**
 * author: brando
 * date: 2/26/24
 */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <bflibcpp/object.hpp>

class SocketConnection : public BF::Object {
public:
	SocketConnection(int sd);
	virtual ~SocketConnection();

private:
	int _id;
	
	/// socket descriptor
	int _sd;
};

#endif // CONNECTION_HPP

