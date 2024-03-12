/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CHAT_ROOM_TESTS_HPP
#define CHAT_ROOM_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "chatroom.hpp"
#include "chatroomserver.hpp"
#include "chatroomclient.hpp"
#include "agentclient.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_chatroominitclient() {
	UNIT_TEST_START;
	int result = 0;
	
	uuid_t u;
	uuid_generate_random(u);	

	AgentClient ac;
	ChatroomClient chatroom(u, &ac);

	UNIT_TEST_END(!result, result);
	return result;
}

int test_chatroominitserver() {
	UNIT_TEST_START;
	int result = 0;
	
	ChatroomServer chatroom;

	UNIT_TEST_END(!result, result);
	return result;
}

int test_chatroomUUIDs() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 18;
	while (!result && max) {
		ChatroomServer c0, c1;

		uuid_t u0, u1;
		c0.getuuid(u0);
		c1.getuuid(u1);

		if (!uuid_compare(u0, u1)) {
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

	LAUNCH_TEST(test_chatroominitclient, p, f);
	LAUNCH_TEST(test_chatroomUUIDs, p, f);
	LAUNCH_TEST(test_chatroominitserver, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CHAT_ROOM_TESTS_HPP

