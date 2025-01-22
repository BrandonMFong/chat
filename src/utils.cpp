/**
 * author: brando
 * date: 1/7/24
 */

#include "utils.hpp"
#include "inputbuffer.hpp"
#include "interface.hpp"

bool Utils::inputReady(InputBuffer & buf, InterfaceState state) {
	return (buf.enterPressed() && (state == kInterfaceStateDraft || buf.starts_with(":")))
		||
		(buf.length() == 1) && !buf.starts_with(":")
		;
}

