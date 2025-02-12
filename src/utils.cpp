/**
 * author: brando
 * date: 1/7/24
 */

#include "utils.hpp"
#include "inputbuffer.hpp"
#include "interface.hpp"

bool Utils::inputReady(InputBuffer & buf, InterfaceState state) {
	return
		buf.starts_with(":") && (buf.length() > 1) && buf.enterPressed()
		||
		!buf.starts_with(":") && (buf.length() == 1) && (state != kInterfaceStateDraft) && (state != kInterfaceStatePromptUsername)
		||
		((state == kInterfaceStateDraft) || (state == kInterfaceStatePromptUsername)) && buf.enterPressed();
		;
}

