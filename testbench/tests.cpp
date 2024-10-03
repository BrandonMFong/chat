/**
 * author: brando
 * date: 2/15/24
 */

#include "inputbuffer_tests.hpp"
#include "chatroom_tests.hpp"
#include "message_tests.hpp"
#include "packet_tests.hpp"
#include "command_tests.hpp"
#include "agent_tests.hpp"
#include "ciphersymmetric_tests.hpp"
#include "cipherasymmetric_tests.hpp"
#include "exception_tests.hpp"
#include "log.hpp"

int main() {
	TEST_SUITE_START;

	/*
	LAUNCH_TEST_SET(inputbuffer_tests);
	LAUNCH_TEST_SET(chatroom_tests);
	*/
	//LAUNCH_TEST_SET(message_tests);
	LAUNCH_TEST_SET(packet_tests);

	/*
	LAUNCH_TEST_SET(command_tests);
	LAUNCH_TEST_SET(agent_tests);
	LAUNCH_TEST_SET(ciphersymmetric_tests);
	LAUNCH_TEST_SET(cipherasymmetric_tests);
	LAUNCH_TEST_SET(exception_tests);
	*/

	TEST_SUITE_END;

	return 0;
}

