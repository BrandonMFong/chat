/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CHAT_ROOM_TESTS_HPP
#define CHAT_ROOM_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "chatroom.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_chatroominit() {
	UNIT_TEST_START;
	int result = 0;

	Chatroom chat;

	UNIT_TEST_END(!result, result);
	return result;
}

int test_chatroomUUIDs() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 18;
	while (!result && max) {
		Chatroom a, b;
		if (!strcmp(a._uuid, b._uuid)) {
			result = max;
		}

		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void chatroom_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_chatroominit, p, f);
	LAUNCH_TEST(test_chatroomUUIDs, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CHAT_ROOM_TESTS_HPP

