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
	 * queues up data to be sent
	 *
	 * data : data to be sent
	 * size : size of data buffer
	 */
	int sendData(const void * data, size_t size);

	/**
	 * sets callback that gets invoked when incoming data is ready to be handled
	 *
	 * callback owner MUST copy buffer data because the data will be lost when it returns
	 */
	void setInStreamCallback(void (* cb)(const void * buf, size_t size));

	/**
	 * see _cbnewconn
	 */
	void setNewConnectionCallback(int (* cb)(int descriptor));

	/**
	 * buffer length for incoming data
	 *
	 * this is the expected size for all incoming data
	 */
	void setBufferSize(size_t size);

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

	/**
	 * callback used, if given, when a new connection is made
	 *
	 * when a new connection is made, socket will invoke callback
	 * to ask caller if connection via socket descriptor is safe
	 * to continue to use.
	 *
	 * Caller may send and recv using this active descriptor and return
	 * nonzero value if there is a problem
	 */
	int (* _cbnewconn)(int sd);

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

	struct Buffer {
		void * data;
		size_t size;
	};

	size_t _bufferSize;

	/**
	 * queues incoming data from recv
	 */	
	BF::Atomic<BF::Queue<struct Buffer *>> _inq;

	/**
	 * queues outgoing data using send
	 */
	BF::Atomic<BF::Queue<struct Buffer *>> _outq;
};

#endif // SOCKET_HPP

