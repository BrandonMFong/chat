/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include <stdlib.h>
#include <bfnet/envelope.hpp>

class User;

namespace BF {
	namespace Net {
		class SocketEnvelope;
	}
}

/**
 * in charge of making packet
 */
namespace Office {

void packetReceive(BF::Net::SocketEnvelope * envelope);

int start();
int stop();

int quitApplication(const User * user);

}

#endif // OFFICE_HPP


