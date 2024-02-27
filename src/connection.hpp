/**
 * author: brando
 * date: 2/26/24
 */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/atomic.hpp>
#include <uuid/uuid.h>

class Socket;

class SocketConnection : public BF::Object {
	friend class Socket;
public:
	static void ReleaseConnection(SocketConnection * sc);

	SocketConnection(int sd);
	virtual ~SocketConnection();

	int descriptor();

	bool isready();
	
	/**
	 * queues up data to be sent
	 *
	 * data : data to be sent
	 * size : size of data buffer
	 */
	int queueData(const void * data, size_t size);

private:
	
	int sendData(const void * buf);

	uuid_t _uuid;
	
	/// socket descriptor
	int _sd;

	/**
	 * true if communication is ready to be made with
	 * our _sd
	 */
	BF::Atomic<bool> _isready;
};

#endif // CONNECTION_HPP

