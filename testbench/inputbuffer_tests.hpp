/**
 * author: Brando
 * date: 2/15/24
 */

#ifndef INPUT_BUFFER_TESTS_HPP
#define INPUT_BUFFER_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "inputbuffer.hpp"
#include <ncurses.h>

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
			result = max * 100 + 1;
		}

		if (!result) {
			buf.addChar(KEY_BACKSPACE);
			str = "hello world";
			if (buf.compareString(str)) {
				printf("\n%s != %s\n", buf.cString(), str);
				result = max * 100 + 2;
			}
		}

		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_inputbufferbackspacechar() {
	UNIT_TEST_START;
	int result = 0;

	InputBuffer buf;
	int max = 2 << 8;
	while (!result && max) {
		int size = 2 << 10;
		for (int i = 0; i < size; i++) {
			buf.addChar('.');
		}

		// test the buffer underflow management
		for (int i = 0; i < (size * 2); i++) {
			buf.addChar(KEY_BACKSPACE);
		}

		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_commandandarg() {
	UNIT_TEST_START;
	int result = 0;

	InputBuffer buf;
	int max = 2 << 8;
	while (!result && max) {
		const char * line = "create room_name";
		for (int i = 0; i < strlen(line); i++) {
			buf.addChar(line[i]);
		}
		
		buf.addChar('\n');

		if (!buf.isready()) {
			result = max;
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
	LAUNCH_TEST(test_inputbufferbackspacechar, p, f);
	LAUNCH_TEST(test_commandandarg, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // INPUT_BUFFER_TESTS_HPP

