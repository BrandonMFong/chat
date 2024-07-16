/**
 * author: brando
 * date: 1/25/24
 */

#ifndef CHAT_HPP
#define CHAT_HPP

#define CHAT_ROOM_MAX_SIZE 5
#define CHAT_SOCKET_SERVER_PORT_NUM 9001
#define CHAT_SOCKET_BUFFER_SIZE sizeof(Packet)

#ifdef MACOS
#ifndef UUID_STR_LEN
#include <uuid/uuid.h>
#define UUID_STR_LEN sizeof(uuid_string_t)
#endif // UUID_STR_LEN
#endif

namespace Chat {
/**
 * returns our main socket
 */
const char SocketGetMode();

int Main(int argc, char * argv[]);

}

#endif // CHAT_HPP

