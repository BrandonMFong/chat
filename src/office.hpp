/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include "typepacket.h"
#include "inputbuffer.hpp"
#include "connection.hpp"

class User;

/**
 * in charge of making packet
 */
namespace Office {

int quitApplication(const User * user);

}

#endif // OFFICE_HPP


