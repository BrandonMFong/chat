/**
 * author: Brando
 * date: 4/7/24
 */

#ifndef COMMAND_TESTS_HPP
#define COMMAND_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "command.hpp"
#include "inputbuffer.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_commandinit, 1, {
	InputBuffer buf;
	Command c(buf);
})

BFTEST_UNIT_FUNC(test_commandop, 2<<10, {
	InputBuffer buf("command subcommand arg0 arg1");
	Command c(buf);

	if (strcmp(c.op(), "command")) {
		result = 1;
	}
})

BFTEST_UNIT_FUNC(test_commandargs, 2<<10, {
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
})

BFTEST_UNIT_FUNC(test_commandargscount, 2<<10, {
	InputBuffer buf("command subcommand arg0 arg1");
	Command c(buf);

	if (c.count() == 0) {
		result = max;
	}
})

BFTEST_COVERAGE_FUNC(command_tests, {
	BFTEST_LAUNCH(test_commandinit);
	BFTEST_LAUNCH(test_commandop);
	BFTEST_LAUNCH(test_commandargs);
	BFTEST_LAUNCH(test_commandargscount);
})

#endif // COMMAND_TESTS_HPP

