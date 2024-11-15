/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CHAT_ROOM_TESTS_HPP
#define CHAT_ROOM_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "chatroom.hpp"
#include "chatroomserver.hpp"
#include "chatroomclient.hpp"
#include "agentclient.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_chatroominitclient, 1, {
	uuid_t u;
	uuid_generate_random(u);	

	AgentClient ac;
	ChatroomClient chatroom(u, &ac);
})

BFTEST_UNIT_FUNC(test_chatroominitserver, 1, {
	ChatroomServer chatroom;
})

BFTEST_UNIT_FUNC(test_chatroomUUIDs, 2<<10, {
	ChatroomServer c0, c1;

	uuid_t u0, u1;
	c0.getuuid(u0);
	c1.getuuid(u1);

	if (!uuid_compare(u0, u1)) {
		result = max;
	}
})

BFTEST_COVERAGE_FUNC(chatroom_tests, {
	BFTEST_LAUNCH(test_chatroominitclient);
	BFTEST_LAUNCH(test_chatroomUUIDs);
	BFTEST_LAUNCH(test_chatroominitserver);
})

#endif // CHAT_ROOM_TESTS_HPP

