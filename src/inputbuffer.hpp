/**
 * author: brando
 * date: 2/14/24
 */

#ifndef INPUT_BUFFER_HPP
#define INPUT_BUFFER_HPP

#include <bflibcpp/string.hpp>
#include "typepacket.h"

class InputBuffer : public BF::String {
public:
	int loadPacketWithBuffer(Packet * pkt);
};

#endif // INPUT_BUFFER_HPP

