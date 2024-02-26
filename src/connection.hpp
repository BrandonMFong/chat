/**
 * author: brando
 * date: 2/26/24
 */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <bflibcpp/object.hpp>
#include <uuid/uuid.h>

class SocketConnection : public BF::Object {
public:
	static void ReleaseConnection(SocketConnection * sc);

	SocketConnection(int sd);
	virtual ~SocketConnection();

	int descriptor() const;
private:
	uuid_t _uuid;
	
	/// socket descriptor
	int _sd;
};

#endif // CONNECTION_HPP

