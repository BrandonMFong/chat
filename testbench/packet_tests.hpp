/**
 * author: Brando
 * date: 10/3/24
 */

#ifndef PACKET_TESTS_HPP
#define PACKET_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "packet.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_settingPayloadMessage, 2<<10, {
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
})

BFTEST_COVERAGE_FUNC(packet_tests, {
	BFTEST_LAUNCH(test_settingPayloadMessage);
})

#endif // PACKET_TESTS_HPP

