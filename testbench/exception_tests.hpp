/**
 * author: Brando
 * date: 7/11/24
 */

#ifndef EXCEPTION_TESTS_HPP
#define EXCEPTION_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "exception.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_exceptions() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		const char * msg = "hello world! i threw an exception!";
		try {
			throw Exception(msg);
		} catch (Exception & e) {
			if (strcmp(e.msg(), msg)) {
				result = max;
			}
		}
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void exception_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_exceptions, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // EXCEPTION_TESTS_HPP

