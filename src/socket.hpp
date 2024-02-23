/**
 * author: brando
 * date: 2/1/24
 */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <bflibcpp/queue.hpp>
#include <bflibcpp/list.hpp>
#include <bflibcpp/atomic.hpp>
#include <bflibcpp/object.hpp>
#include <bflibcpp/array.hpp>
#include <typepacket.h>

extern "C" {
#include <bflibc/thread.h>
}

#define SOCKET_MODE_SERVER 's'
#define SOCKET_MODE_CLIENT 'c'

class Socket : public BF::Object {
public: 
	static Socket * shared();

	static Socket * create(const char mode, int * err);
	virtual ~Socket();

	virtual const char mode() const = 0;

	int start();
	int stop();
	
	/**
	 * queues up packet to be sent
	 */
	int sendPacket(const Packet * packet);

	/**
	 * sets callback that gets invoked when incoming data is ready to be handled
	 */
	void setInStreamCallback(void (* cb)(const void * buf, size_t size));

protected:
	Socket();

	// _start & _stop gets called at the start
	// of the start() and stop() respectively
	virtual int _start() = 0;
	virtual int _stop() = 0;

	/**
	 * clients must call this function at the end of _start()
	 *
	 * 'sd' : socket descriptor
	 */
	int startInStreamForConnection(int sd);

	/**
	 * array of devices we are connected to
	 */
	BF::Atomic<BF::Array<int>> _connections;

private:

	/**
	 * handles received packets from queue
	 */
	static void queueCallback(void * in);

	/**
	 * call back that gets called in `queueCallback` when it
	 * pops data from in q
	 */
	void (* _cbinstream)(const void * buf, size_t size);

	/**
	 * receives packets and puts them in a queue
	 */
	static void inStream(void * in);

	/**
	 * sends packets out
	 */
	static void outStream(void * in);

	BF::Atomic<BF::List<BFThreadAsyncID>> _tidin;
	BFThreadAsyncID _tidq;
	BFThreadAsyncID _tidout;

	/**
	 * queues incoming data from recv
	 */	
	BF::Atomic<BF::Queue<Packet *>> _inq;

	/**
	 * queues outgoing data using send
	 */
	BF::Atomic<BF::Queue<Packet *>> _outq;
};

#endif // SOCKET_HPP

