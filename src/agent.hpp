/**
 * author: brando
 * date: 2/26/24
 */

#ifndef AGENT_HPP
#define AGENT_HPP

#include <bflibcpp/object.hpp>
#include "connection.hpp"

class Agent : public BF::Object {
public:
	Agent(const SocketConnection * sc);
	virtual ~Agent();

private:
	const SocketConnection * _sc;
};

#endif // AGENT_HPP

