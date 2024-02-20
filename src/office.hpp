/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include "typepacket.h"

namespace Office {

/**
 * handles incoming packets
 *
 * packet gets passed from socket to us
 */
int PacketReceive(const Packet * pkt);

}

#endif // OFFICE_HPP


