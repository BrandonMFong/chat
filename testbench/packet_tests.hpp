/**
 * author: Brando
 * date: 10/3/24
 */

#ifndef PACKET_TESTS_HPP
#define PACKET_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "packet.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_settingPayloadMessage() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		PayloadMessage payload;
		uuid_t uuidu, uuidc;
		uuid_generate_random(uuidu);
		uuid_generate_random(uuidc);
		const char * username = "username";
		InputBuffer buf("hello world");

		PacketPayloadSetPayloadMessage(
			&payload,
			kPayloadMessageTypeData,
			uuidc, username, uuidu,
			buf);

		Data data = buf;

		// now check
		if (payload.type != kPayloadMessageTypeData) {
			result = 1;
		} else if (uuid_compare(payload.chatuuid, uuidc)) {
			result = 2;
		} else if (strcmp(payload.username, username)) {
			result = 3;
		} else if (uuid_compare(payload.useruuid, uuidu)) {
			result = 4;
		} else if (memcmp(payload.data, data.buffer(), data.size())) {
			result = 5;
		}
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void packet_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_settingPayloadMessage, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // PACKET_TESTS_HPP

