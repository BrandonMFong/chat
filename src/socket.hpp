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

extern "C" {
#include <bflibc/thread.h>
}

#define SOCKET_MODE_SERVER 's'
#define SOCKET_MODE_CLIENT 'c'

class SocketConnection;

class Socket : public BF::Object {
	friend class SocketConnection;

public: 
	static Socket * shared();

	static Socket * create(const char mode, int * err);
	virtual ~Socket();

	/**
	 * returns: SOCKET_MODE_SERVER if we are a server or
	 * SOCKET_MODE_CLIENT if we are a client
	 */
	virtual const char mode() const = 0;

	int start();
	int stop();
	
	/**
	 * queues up data to be sent
	 *
	 * data : data to be sent
	 * size : size of data buffer
	 */
	//int sendData(const void * data, size_t size);

	/**
	 * sets callback that gets invoked when incoming data is ready to be handled
	 *
	 * callback owner MUST copy buffer data because the data will be lost when it returns
	 */
	void setInStreamCallback(void (* cb)(SocketConnection * sc, const void * buf, size_t size));

	/**
	 * see _cbnewconn
	 */
	void setNewConnectionCallback(int (* cb)(SocketConnection * sc));

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
	int startInStreamForConnection(SocketConnection * sc);

	/**
	 * array of devices we are connected to
	 */
	BF::Atomic<BF::List<SocketConnection *>> _connections;

	/**
	 * callback used, if given, when a new connection is made
	 *
	 * sc : keep a record of this if you want to send data to the 
	 * device on the other end.  You do not own memory
	 */
	int (* _cbnewconn)(SocketConnection * sc);

private:

	/**
	 * handles received packets from queue
	 */
	static void queueCallback(void * in);

	/**
	 * call back that gets called in `queueCallback` when it
	 * pops data from in q
	 */
	void (* _cbinstream)(SocketConnection * sc, const void * buf, size_t size);

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
	 * buffer for incoming and outgoing data
	 */
	struct Buffer {
		void * data;
		size_t size;
	};

	/**
	 * ties relationship for buffer data with socket connection
	 *
	 * that way outStream thread knows who to send
	 * buffer data to
	 */
	struct Envelope {
		SocketConnection * sc;
		struct Buffer buf;
	};

	/**
	 * holds expected buffer size for all incoming and outcoming data
	 *
	 * implementer is responsible for setting this
	 */
	size_t _bufferSize;

	/**
	 * queues incoming data from recv
	 */	
	BF::Atomic<BF::Queue<struct Envelope *>> _inq;

	/**
	 * queues outgoing data using send
	 */
	BF::Atomic<BF::Queue<struct Envelope *>> _outq;
};

#endif // SOCKET_HPP

