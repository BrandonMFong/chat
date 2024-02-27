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
	Agent(const SocketConnection * sc);
	virtual ~Agent();

private:
	/*
	 * socket connection
	 */
	const SocketConnection * _sc;
};

#endif // AGENT_HPP

