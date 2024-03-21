/**
 * author: brando
 * date: 3/13/24
 */

#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <bflibcpp/object.hpp>
#include "buffer.hpp"

class SocketConnection;
class Socket;

class SocketEnvelope : public BF::Object {
	friend class Socket;
public:
	SocketEnvelope(SocketConnection * sc, size_t bufsize);
	virtual ~SocketEnvelope();

	SocketBuffer * buf();
	SocketConnection * connection();
private:
	SocketConnection * _sc;
	SocketBuffer _buf;
};

#endif // ENVELOPE_HPP

