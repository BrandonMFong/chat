/**
 * author: brando
 * date: 1/7/24
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "typeinterfacestate.hpp"

class InputBuffer;

namespace Utils {
bool inputReady(InputBuffer & buf, InterfaceState state);
}

#endif // UTILS_HPP

