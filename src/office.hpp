/**
 * author: brando
 * date: 2/20/24
 */

#ifndef OFFICE_HPP
#define OFFICE_HPP

#include <stdlib.h>

class User;
class SocketConnection;

/**
 * in charge of making packet
 */
namespace Office {

typedef struct {
	SocketConnection * sc;
	void * buf;
	size_t size;
} InData;

void packetReceive(SocketConnection * sc, const void * buf, size_t size);

int start();
int stop();

int quitApplication(const User * user);

}

#endif // OFFICE_HPP


