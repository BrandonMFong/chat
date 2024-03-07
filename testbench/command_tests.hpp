/**
 * author: Brando
 * date: 4/7/24
 */

#ifndef COMMAND_TESTS_HPP
#define COMMAND_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "command.hpp"
#include "inputbuffer.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_commandinit() {
	UNIT_TEST_START;
	int result = 0;

	InputBuffer buf;
	Command c(buf);

	UNIT_TEST_END(!result, result);
	return result;
}

void command_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_commandinit, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // COMMAND_TESTS_HPP

