/**
 * author: Brando
 * date: 2/15/24
 */

#ifndef INPUT_BUFFER_TESTS_HPP
#define INPUT_BUFFER_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "inputbuffer.hpp"
#include <ncurses.h>

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_inputbufferinit, 1, {
	InputBuffer buf;
})

BFTEST_UNIT_FUNC(test_inputbuffermodifiers, 2<<10, {
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
})

BFTEST_UNIT_FUNC(test_inputbufferbackspacechar, 2<<10, {
	InputBuffer buf;
	int size = 2 << 10;
	for (int i = 0; i < size; i++) {
		buf.addChar('.');
	}

	// test the buffer underflow management
	for (int i = 0; i < (size * 2); i++) {
		buf.addChar(KEY_BACKSPACE);
	}
})

BFTEST_UNIT_FUNC(test_commandandarg, 2<<10, {
	InputBuffer buf;
	const char * line = "create room_name";
	for (int i = 0; i < strlen(line); i++) {
		buf.addChar(line[i]);
	}
	
	buf.addChar('\n');

	if (!buf.isready()) {
		result = max;
	}
})

BFTEST_COVERAGE_FUNC(inputbuffer_tests, {
	BFTEST_LAUNCH(test_inputbufferinit);
	BFTEST_LAUNCH(test_inputbuffermodifiers);
	BFTEST_LAUNCH(test_inputbufferbackspacechar);
	BFTEST_LAUNCH(test_commandandarg);
})

#endif // INPUT_BUFFER_TESTS_HPP

