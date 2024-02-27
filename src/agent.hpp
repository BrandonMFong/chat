/**
 * author: brando
 * date: 2/26/24
 */

#ifndef AGENT_HPP
#define AGENT_HPP

#include <bflibcpp/object.hpp>
#include "connection.hpp"

/**
 * Represents the remote user
 */
class Agent : public BF::Object {
public:
	/**
	 * we do not own the socket connection object
	 */
	Agent(SocketConnection * sc);
	virtual ~Agent();

	/**
	 * starts a conversation with the user on the other end
	 * to get to know them
	 */
	int start();

private:

	/**
	 * waits for the socket connection to be ready before start
	 * a conversation with the remote user
	 */
	static void handshake(void * in);

	/*
	 * socket connection
	 *
	 * the socket descriptor that is wrapped within this object
	 * represents to user on other end we are representing
	 */
	SocketConnection * _sc;
};

#endif // AGENT_HPP

