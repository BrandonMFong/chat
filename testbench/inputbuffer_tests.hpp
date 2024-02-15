/**
 * author: Brando
 * date: 2/15/24
 */

#ifndef INPUT_BUFFER_TESTS_HPP
#define INPUT_BUFFER_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "inputbuffer.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_inputbufferinit() {
	UNIT_TEST_START;
	int result = 0;

	InputBuffer buf;

	UNIT_TEST_END(!result, result);
	return result;
}

int test_inputbuffermodifiers() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 14;
	while (!result && max) {
		InputBuffer buf;
		const char * str = "hello world!";
		for (int i = 0; i < strlen(str); i++) {
			buf.addChar(str[i]);
		}

		if (buf.compareString(str)) {
			result = 1;
		}

		if (!result) {
			buf.addChar('\a');
			str = "hello world";
			if (buf.compareString(str)) {
				printf("\n%s != %s\n", buf.cString(), str);
				result = 2;
			}
		}

		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void inputbuffer_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_inputbufferinit, p, f);
	LAUNCH_TEST(test_inputbuffermodifiers, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // INPUT_BUFFER_TESTS_HPP

