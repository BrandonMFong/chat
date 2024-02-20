/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include "typepacket.h"
#include "typemessage.h"
#include "inputbuffer.hpp"

namespace Office {

/**
 * handles incoming packets
 *
 * packet gets passed from socket to us
 */
int PacketReceive(const Packet * pkt);

/**
 */
int BufferSend(const InputBuffer * buf);

}

#endif // OFFICE_HPP


