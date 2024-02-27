/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include "typepacket.h"
#include "inputbuffer.hpp"
#include "connection.hpp"

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

/**
 * this is a callback described by the Socket family
 *
 * this will get called when a new connection is made
 * by the socket. 
 *
 * in this function we will create an agent that represents
 * the conversation between us and the user on the other end.
 *
 * Once created, the agent will get to know the user on ther
 * other end and get them ready to join a conversation.
 */
int NewConnection(const SocketConnection * sc);

}

#endif // OFFICE_HPP


