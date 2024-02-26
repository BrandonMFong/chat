/**
 * author: brando
 * date: 2/22/24
 */

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "socket.hpp"

/**
 * protocol used with be SIMPLE
 * ref: https://en.wikipedia.org/wiki/Session_Initiation_Protocol
 */
namespace Protocol {

int Handshake(const SocketConnection * d);

}

#endif // PROTOCOL_HPP

