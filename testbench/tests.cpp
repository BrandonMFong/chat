/**
 * author: brando
 * date: 2/15/24
 */

#include "inputbuffer_tests.hpp"
#include "chatroom_tests.hpp"
#include "message_tests.hpp"
#include "log.hpp"

LOG_INIT

int main() {
	TEST_SUITE_START;

	LAUNCH_TEST_SET(inputbuffer_tests);
	LAUNCH_TEST_SET(chatroom_tests);
	LAUNCH_TEST_SET(message_tests);

	TEST_SUITE_END;

	return 0;
}

