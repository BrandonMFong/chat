/**
 * author: brando
 * date: 1/25/24
 */

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "socket.hpp"
#include <typepacket.h>

int InterfaceRun(Socket * skt);

void InterfaceInStreamQueueCallback(const Packet & p);

#endif // INTERFACE_HPP

