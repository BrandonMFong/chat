/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include "typepacket.h"
#include "inputbuffer.hpp"

/**
 * in charge of making packet
 */
namespace Office {

/**
 * handles incoming packets
 *
 * packet gets passed from socket to us
 */
void PacketReceive(const void * buf, size_t size);

/**
 */
int PacketSend(const Packet * pkt);

}

#endif // OFFICE_HPP


