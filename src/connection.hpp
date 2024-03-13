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
class Client;
class Client;
class SocketBuffer;

class SocketConnection : public BF::Object {
	friend class Socket;
	friend class Server;
	friend class Client;

public:
	static void ReleaseConnection(SocketConnection * sc);

	/**
	 * if true, then we are now able to send data
	 * to remote user
	 */
	bool isready();
	
	/**
	 * queues up data to be sent
	 *
	 * data : data to be sent.  this data is copied. Caller still owns
	 * size : size of data buffer
	 */
	int queueData(const void * data, size_t size);

	/**
	 * see Socket::mode
	 */
	const char mode();

	/**
	 * returns uuid
	 */
	void getuuid(uuid_t uuid);

private:

	/**
	 * sktref : reference to socket
	 */
	SocketConnection(int sd, Socket * sktref);
	virtual ~SocketConnection();

	/**
	 * closes socket descriptor
	 */
	void closeConnection();

	int sendData(const SocketBuffer * buf);
	int recvData(SocketBuffer * buf);

	/**
	 * uniquely ids the connection
	 */
	uuid_t _uuid;
	
	/// socket descriptor
	int _sd;

	/**
	 * true if communication is ready to be made with
	 * our _sd
	 */
	BF::Atomic<bool> _isready;

	/**
	 * reference to socket
	 */
	Socket * _sktref;
};

#endif // CONNECTION_HPP

