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

		uuid_t uuidu, uuidc;
		uuid_generate_random(uuidu);
		uuid_generate_random(uuidc);

		p.payload.message.type = kPayloadMessageTypeData;
		strncpy(p.payload.message.data, "hello world", sizeof(p.payload.message.data));
		strncpy(p.payload.message.username, "username", sizeof(p.payload.message.username));
		uuid_copy(p.payload.message.useruuid, uuidu);
		uuid_copy(p.payload.message.chatuuid, uuidc);
		p.header.time = BFTimeGetCurrentTime();

		Message * m = new Message(&p);
		if (!m)
			result = max;

		if (!result) {
			uuid_t u0, u1;
			m->getuuidchatroom(u0);
			m->getuuiduser(u1);
			if (strcmp(m->data(), p.payload.message.data)) {
				result = max;
			} else if (strcmp(m->username(), p.payload.message.username)) {
				result = max;
			} else if (uuid_compare(u0, uuidc)) {
				result = max;
			} else if (uuid_compare(u1, uuidu)) {
				result = max;
			} else if (m->type() != kPayloadMessageTypeData) {
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

