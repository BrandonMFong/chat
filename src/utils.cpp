/**
 * author: brando
 * date: 1/7/24
 */

#include "utils.hpp"
#include "inputbuffer.hpp"
#include "interface.hpp"

bool Utils::inputReady(InputBuffer & buf) {
	return buf.enterPressed() && (Interface::current()->currstate() == kInterfaceStateDraft || buf.starts_with(":"));
}

