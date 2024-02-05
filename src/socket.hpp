/**
 * author: brando
 * date: 2/1/24
 */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <bflibcpp/queue.hpp>
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/object.hpp>
#include <typepacket.h>

extern "C" {
#include <bflibc/thread.h>
}

#define SOCKET_MODE_SERVER 's'
#define SOCKET_MODE_CLIENT 'c'

class Socket : public BF::Object {
public: 
	static Socket * create(const char mode, int * err);
	virtual ~Socket();

	virtual const char mode() const = 0;

	int start();
	int stop();
	
	static void inStream(void * in);
	static void outStream(void * in);

	BF::Atomic<BF::Queue<Packet *>> in;
	BF::Atomic<BF::Queue<Packet *>> out;
	int sendPacket(const Packet * packet);
	void setInStreamCallback(void (* callback)(const Packet &));

	virtual const int descriptor() const = 0;

protected:
	Socket();

	// _start & _stop gets called at the start
	// of the start() and stop() respectively
	virtual int _start() = 0;
	virtual int _stop() = 0;

	/**
	 * clients must call this function at the end of _start()
	 */
	int startIOStreams();

private:
	BFThreadAsyncID _tidin;
	BFThreadAsyncID _tidout;
};

#endif // SOCKET_HPP

