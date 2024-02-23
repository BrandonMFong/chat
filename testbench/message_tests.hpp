/**
 * author: Brando
 * date: 2/15/24
 */

#ifndef MESSAGE_TESTS_HPP
#define MESSAGE_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "message.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_messageinit() {
	UNIT_TEST_START;
	int result = 0;

	Packet p;
	Message * m = new Message(&p);
	Delete(m);

	UNIT_TEST_END(!result, result);
	return result;
}

int test_packet2message() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 8;
	while (!result && max) {
		Packet p;
		memset(&p, 0, sizeof(p));

		strncpy(p.data, "hello world", sizeof(p.data));
		strncpy(p.header.username, "username", sizeof(p.header.username));
		strncpy(p.header.useruuid, "uuid", kBFStringUUIDStringLength);
		strncpy(p.header.chatuuid, "uuid", kBFStringUUIDStringLength);
		p.header.time = BFTimeGetCurrentTime();

		Message * m = new Message(&p);
		if (!m)
			result = max;

		if (!result) {
			if (strcmp(m->data(), p.data)) {
				result = max;
			} else if (strcmp(m->username(), p.header.username)) {
				result = max;
			} else if (strcmp(m->chatuuid(), p.header.chatuuid)) {
				result = max;
			}
		}

		Delete(m);

		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;

}

void message_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_messageinit, p, f);
	LAUNCH_TEST(test_packet2message, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // MESSAGE_TESTS_HPP

