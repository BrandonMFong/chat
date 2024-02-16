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
	InputBuffer();
	virtual ~InputBuffer();

	/**
	 * unloads buffer to the packet
	 */
	int unload(Packet * pkt);

	/**
	 * Adds appends char to our buffer
	 * Adjusts the cursor position accordingly
	 */
	virtual int addChar(int c);

	/**
	 * when the buffer is ready to be sent
	 */
	bool isready();

	/**
	 * clears buffer and resets the cursor position
	 */	
	virtual int clear();

	/**
	 * returns cursor position
	 */
	size_t cursorPosition();

private:

	bool _isready;

	/**
	 * current cursor position
	 */
	size_t _cursorpos;
};

#endif // INPUT_BUFFER_HPP

