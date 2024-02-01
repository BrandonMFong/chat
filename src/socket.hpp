/**
 * author: brando
 * date: 2/1/24
 */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "typechatconfig.h"
#include <bflibcpp/queue.hpp>
#include <bflibcpp/atomic.hpp>
#include <typepacket.h>

#define SOCKET_MODE_SERVER 's'
#define SOCKET_MODE_CLIENT 'c'

class Socket {
public: 
	static Socket * create(const char mode, int * err);
	virtual ~Socket();

	virtual int start() = 0;
	virtual int stop() = 0;

	virtual const char mode() const = 0;
	
	static void inStream(void * in);
	static void outStream(void * in);

	BF::Atomic<BF::Queue<Packet *>> in;
	BF::Atomic<BF::Queue<Packet *>> out;

	int sockd;

protected:
	Socket();
};

#endif // SOCKET_HPP

