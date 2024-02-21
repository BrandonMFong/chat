/**
 * author: Brando
 * date: 2/21/24
 */

#ifndef CHAT_DIRECTORY_TESTS_HPP
#define CHAT_DIRECTORY_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "chatdirectory.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_chatdirectoryinit() {
	UNIT_TEST_START;
	int result = 0;

	ChatDirectory cd;

	UNIT_TEST_END(!result, result);
	return result;
}

int test_findingchatroom() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 16;
	while (!result && max) {
		ChatDirectory cd;
		Chatroom r0, r1;
		cd.addChatroom(&r0);
		cd.addChatroom(&r1);

		Chatroom * _r0, * _r1;

		_r0 = cd.getChatroom(r0.uuid());
		_r1 = cd.getChatroom(r1.uuid());

		if (!_r0) {
			result = 1;
		} else if (!_r1) {
			result = 2;
		} else if (BFStringCompareUUID(_r0->uuid(), r0.uuid())) {
			result = 3;
		} else if (BFStringCompareUUID(_r1->uuid(), r1.uuid())) {
			result = 4;
		}
		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;

}

void chatdirectory_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_chatdirectoryinit, p, f);
	LAUNCH_TEST(test_findingchatroom, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CHAT_DIRECTORY_TESTS_HPP

