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

int test_commandop() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 8;
	while (!result && max--) {
		InputBuffer buf("command subcommand arg0 arg1");
		Command c(buf);

		if (strcmp(c.op(), "command")) {
			result = 1;
		}
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_commandargs() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 8;
	while (!result && max--) {
		InputBuffer buf("command subcommand arg0 arg1");
		Command c(buf);

		if (strcmp(c.op(), "command")) {
			result = 1;
		} else if (strcmp(c[1], "subcommand")) {
			result = 2;
		} else if (strcmp(c[2], "arg0")) {
			result = 3;
		} else if (strcmp(c[3], "arg1")) {
			result = 4;
		}
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_commandargscount() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 8;
	while (!result && max--) {
		InputBuffer buf("command subcommand arg0 arg1");
		Command c(buf);

		if (c.count() == 0) {
			result = max;
		}
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void command_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_commandinit, p, f);
	LAUNCH_TEST(test_commandop, p, f);
	LAUNCH_TEST(test_commandargs, p, f);
	LAUNCH_TEST(test_commandargscount, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // COMMAND_TESTS_HPP

