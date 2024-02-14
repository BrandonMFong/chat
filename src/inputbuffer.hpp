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
	/**
	 * unloads buffer to the packet
	 */
	int unload(Packet * pkt);

	virtual int addChar(char c);

	/**
	 * when the buffer is ready to be sent
	 */
	bool isready();

private:

	bool _isready = false;
};

#endif // INPUT_BUFFER_HPP

