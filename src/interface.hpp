/**
 * author: brando
 * date: 1/25/24
 */

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "socket.hpp"
#include <typepacket.h>

class User;

namespace Interface {

int Run();
const User * GetCurrentUser();

}

#endif // INTERFACE_HPP

