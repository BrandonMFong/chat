/**
 * author: brando
 * date: 4/13/24
 */

#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include <bflibcpp/object.hpp>

class SocketConnection;

class SocketEnvelope : public BF::Object {
public:
	SocketEnvelope(SocketConnection * sc, size_t bufsize);
	virtual ~SocketEnvelope();

	/**
	 * buf : gets copied
	 */
	void setbuf(const void * buf);

	const void * buf();
	size_t bufsize();
	SocketConnection * connection();
private:
	SocketConnection * _sc;
	void * _buf;
	size_t _bufsize;
};

#endif // ENVELOPE_HPP

