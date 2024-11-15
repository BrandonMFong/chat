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
#include "log.hpp"

BFTEST_SUITE_FUNC({
	BFTEST_SUITE_LAUNCH(inputbuffer_tests);
	BFTEST_SUITE_LAUNCH(chatroom_tests);
	BFTEST_SUITE_LAUNCH(message_tests);
	BFTEST_SUITE_LAUNCH(packet_tests);
	BFTEST_SUITE_LAUNCH(command_tests);
	BFTEST_SUITE_LAUNCH(agent_tests);
	BFTEST_SUITE_LAUNCH(ciphersymmetric_tests);
	BFTEST_SUITE_LAUNCH(cipherasymmetric_tests);
})

