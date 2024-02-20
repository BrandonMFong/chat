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
	 * sets the callback that will be in charge of observing
	 * the top the of the in stream packet queue when available
	 */
	void setQueueCallback(void (* callback)(const Packet &));

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

	/**
	 * handles received packets from queue
	 */
	static void queueCallback(void * in);

	/**
	 * receives packets
	 */
	static void inStream(void * in);

	/**
	 * sends packets out
	 */
	static void outStream(void * in);

	BFThreadAsyncID _tidin;
	BFThreadAsyncID _tidinpop;
	BFThreadAsyncID _tidout;

	/**
	 * flags to all the stream threads whether
	 * the caller wants it to stop running
	 */
	BF::Atomic<bool> _stopStreams;

	/**
	 * callback that will observe packet from
	 * top of the in stream queue	
	 */
	void (* _callback)(const Packet &);

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

