/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include "typepacket.h"
#include "typemessage.h"

namespace Office {

/**
 * handles incoming packets
 *
 * packet gets passed from socket to us
 */
int PacketReceive(const Packet * pkt);

/**
 * takes message and creates a packet to 
 * be sent by socket to our receivers
 */
int MessageSend(const Message * msg);

}

#endif // OFFICE_HPP


