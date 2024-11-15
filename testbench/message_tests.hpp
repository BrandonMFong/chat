/**
 * author: Brando
 * date: 2/15/24
 */

#ifndef MESSAGE_TESTS_HPP
#define MESSAGE_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "message.hpp"
#include "packet.hpp"
#include "cipher.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_messageinit, 1, {
	Packet p;
	Message * m = new Message(&p);
	BFRelease(m);
})

void _test_messagesLoadRandomDataToMessagePayload(Packet * p, const char * message) {
	memset(p, 0, sizeof(Packet));

	uuid_t uuidu, uuidc;
	uuid_generate_random(uuidu);
	uuid_generate_random(uuidc);

	PacketPayloadSetPayloadMessage(
		&p->payload.message,
		kPayloadMessageTypeData,
		uuidc, "username", uuidu,
		message);
}

BFTEST_UNIT_FUNC(test_packet2message, 2<<10, {
	Packet p;
	_test_messagesLoadRandomDataToMessagePayload(&p, "hello world");

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
		} else if (uuid_compare(u0, p.payload.message.chatuuid)) {
			result = max;
		} else if (uuid_compare(u1, p.payload.message.useruuid)) {
			result = max;
		} else if (m->type() != kPayloadMessageTypeData) {
			result = max;
		}
	}

	BFRelease(m);
})

BFTEST_UNIT_FUNC(test_messageEncryptAndDecrypt, 2<<10, {
	Packet p;
	String str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
	_test_messagesLoadRandomDataToMessagePayload(&p, str.cString());
	Message * m = new Message(&p);
	if (!m) {
		result = 1;
		break;
	}

	Cipher * cipher = Cipher::create(kCipherTypeSymmetric);
	if (!cipher) {
		result = 2;
		break;
	} else if (cipher->genkey()) {
		result = 3;
		break;
	}

	if (m->encryptData(cipher)) {
		result = 3;
		break;
	}

	if (m->decryptData(cipher)) {
		result = 4;
		break;
	}

	BFRelease(m);
	BFRelease(cipher);
})

BFTEST_COVERAGE_FUNC(message_tests, {
	BFTEST_LAUNCH(test_messageinit);
	BFTEST_LAUNCH(test_packet2message);
	BFTEST_LAUNCH(test_messageEncryptAndDecrypt);
})

#endif // MESSAGE_TESTS_HPP

